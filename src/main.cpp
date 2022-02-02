#include <Arduino.h>

#include <GyverMAX7219.h>
#define AM_W 16  // 2 матрицы (18 точки)
#define AM_H 8  // 1 матрица (8 точек)

#define J_VRX 0
#define J_VRY 1
#define J_SW 3

MAX7219 < 2, 1, 5 > mtrx;   // одна матрица (1х1), пин CS на D5

int snake_size, change_x, change_y, coordinates_x[128], coordinates_y[128];
int food_x = -1, food_y = -1;
char symbol;
int a[16][8];
const int N = 8, M = 16, INTERVAL = 200;

void change_direction() {
  int x, y;
  x = analogRead(J_VRX);
  y = analogRead(J_VRY);
  if(x > 900) symbol = 'd';
  if(x < 100) symbol = 'a';
  if(y < 100) symbol = 's';
  if(y > 900) symbol = 'w';
  switch (symbol) {
    // Управление змейкой у нас через wasd.
    case 'w': if (change_x != 1 || change_y != 0) {
                change_x = -1; change_y = 0;
              }
              break;
    case 'a': if (change_x != 0 || change_y != 1) {
                change_x = 0; change_y = -1;
              }
              break;
    case 's': if (change_x != -1 || change_y != 0) {
                change_x = 1; change_y = 0;
              }
              break;
    case 'd': if (change_x != 0 || change_y != -1) {
                change_x = 0; change_y = 1;
              }
              break;
    default: break;
  }
}

// функция для вывода таблицы
void show_table() {
  // Очищаем консоль.
  mtrx.clear();
  for(int i = 1 ; i <= snake_size; i++) {
    mtrx.dot(coordinates_x[i],coordinates_y[i]);
  }
  mtrx.update();
    
}

// Очищаем координаты, в которых располагалась змейка.
void clear_snake_on_table() {
  for (int i = 1; i <= snake_size; ++i)
    a[coordinates_x[i]][coordinates_y[i]] = 0;
}

// Красим координаты змейки.
void show_snake_on_table() {
  // Красим змейку.
  for (int i = 1; i <= snake_size; ++i)
    a[coordinates_x[i]][coordinates_y[i]] = 1;
}

// Проверяем, съела ли змейка саму себя.
bool game_over() {
  for (int i = 2; i <= snake_size; ++i)
    // Eсли координаты головы змейки равны координате какой-либо части тела
    // змейки, то змейка съела саму себя.
    if (coordinates_x[1] == coordinates_x[i] &&
        coordinates_y[1] == coordinates_y[i])
      return true;
  // Если все координаты различны, то все в порядке - играем дальше.
  return false;
}

// Проверяем, не вышла ли змейка за поле, если да то возвращаем ее обратно.
void check_coordinates() {
  if (coordinates_x[1] > N) coordinates_x[1] = 1;
  if (coordinates_x[1] < 1) coordinates_x[1] = N;
  if (coordinates_y[1] > M) coordinates_y[1] = 1;
  if (coordinates_y[1] < 1) coordinates_y[1] = M;
}

// функция следующего хода, в которой наша змейка сдвигается в сторону
// на 1 ячейку.
void next_step() {
  // Чистим таблицу от змейки.
  clear_snake_on_table();

  // Передвигаем тело змейки.
  for (int i = snake_size; i >= 2; --i) {
    coordinates_x[i] = coordinates_x[i - 1];
    coordinates_y[i] = coordinates_y[i - 1];
  }

  // Передвигаем голову змейки.
  coordinates_x[1] += change_x;
  coordinates_y[1] += change_y;

  // Проверяем в порядке ли координаты.
  check_coordinates();

  // Если голова змейки там же где и еда, то увеличиваем размер змейки
  // и очищаем координаты змейки.
  if (coordinates_x[1] == food_x && coordinates_y[1] == food_y) {
    snake_size++;
    food_x = -1;
    food_y = -1;
  }

  // Рисуем змейку.
  show_snake_on_table();

  // Если змея укусила себя.
  if (game_over()) {
    // Cообщаем всю правду о игроке.
    mtrx.clear();
    // Приостанавливаем игру.
    while(!analogRead(J_SW)) delay(100);

    // Завершаем программу.
    
  }
}

// функция добавления еды на карту.
void place_food() {
  // Cтавим в рандомное место еду.
  for (int i = 1; i <= 9; ++i) {
    int x = random(N), y = random(M);
    if (x < 0) x *= -1;
    if (y < 0) y *= -1;
    x %= (N + 1);
    y %= (M + 1);
    if (x == 0) ++x;
    if (y == 0) ++y;
    if (a[x][y] != 1) {
      food_x = x;
      food_y = y;
      a[x][y] = 2;
      return;
    }
  }
}

// функция проверки на наличие еды на карте.
bool food_check() {
  // Если координаты еды неопределенны, то возвращаем true.
  if (food_x == -1 && food_y == -1) return false;
  // В остальных случаях false.
  return true;
}

// Начальные установки.
void standart_settings() {
  // Размер змеи - 2.
  snake_size = 2;

  // Змейка занимает две клетки вправо от координаты {1,1}.
  coordinates_x[1] = 5;
  coordinates_y[1] = 5;
  coordinates_x[2] = 4;
  coordinates_y[2] = 5;

  // Змейка движется вправо.
  change_x = 1;
  change_y = 0;
}

void setup() {
  mtrx.begin();       // запускаем
  mtrx.setBright(1);  // яркость 0..15
  //mtrx.setRotation(1);   // можно повернуть 0..3, по 90 град по часовой стрелке
  Serial.begin(9600);
  pinMode(J_SW, INPUT_PULLUP);
  
}

void loop() {
  
  standart_settings();

  while (true) {
    // Если нажата клавиша, обрабатываем нажатую клавишу.
    //if (kbhit() != 0)
     change_direction();

    // Двигаем змейку.
    next_step();

    // Если нет еды, то ставим ее.
    if (!food_check())
      place_food();

    // Рисуем змейку.
    show_table();

    // "Усыпляем" программу на заданный интервал.
    delay(INTERVAL);
  }


  
  

}