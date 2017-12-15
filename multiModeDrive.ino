#include <IRremote.h> //Подключение библиотеки для получения и обработки сигнала с ИК-приемника.

const int R_barrierSensor = 4; //Вывод, к которому подключен правый датчик препятствия.
const int RC_barrierSensor = 5; //Вывод, к которому подключен центральный правый датчик препятствия.
const int LC_barrierSensor = 6; //Вывод, к которому подключен центральный левый датчик препятствия.
const int L_barrierSensor = 7; //Вывод, к которому подключен левый датчик препятствия.

#define E1 3 //Вывод платы расширения Motor Drive Shield, используемый для контроля скорости мотора А.
#define E2 11 //Вывод платы расширения Motor Drive Shield, используемый для контроля скорости мотора В.
#define M1 12 //Вывод платы расширения Motor Drive Shield, используемый для контроля направления вращения мотора А.
#define M2 13 //Вывод платы расширения Motor Drive Shield, используемый для контроля направления вращения мотора В.
//Объявление переменных
boolean R_free = false; //Правый датчик.
boolean RC_free = false; //Центральный правый датчик.
boolean LC_free = false; //Центральный левый датчик.
boolean L_free = false; //Левый датчик.
int motorSpeed = 80; //Переменная для хранения скорости вращения моторов (вы можете задать свою скорость вращения колес).

enum modeType {
  menu,
  remote,
  selfRun,
  programEdit,
  programRun,
};
modeType mode = remote;

int IR_pin = 8; //Вывод, к которому подключен ИК-приемник.
decode_results results; //Переменная для хранения принятых данных (кодов нажатых кнопок на пульте).

IRrecv irreceiver(IR_pin); //Создание объекта приемника, имеющего специфичные для приемника свойства и методы.
//Например, метод resume() возобновляет получение данных приемником после декодирования результатов.

//Вспомогательные переменные (используются при попадании робота в тупик).
boolean movingRight = true;
int tryes = 0;

void motor_drive_config() //Инициализация выводов платы расширения Motor Drive Shield.
{
  pinMode(M1, OUTPUT); //Назначение вывода M1 выходом.
  pinMode(M2, OUTPUT); //Назначение вывода M2 выходом.
  pinMode(E1, OUTPUT); //Назначение вывода E1 выходом.
  pinMode(E2, OUTPUT); //Назначение вывода E2 выходом.
}

void scan_barrier_sensors() { //Получение информации о наличии препятствий в заданных направлениях.
  R_free = digitalRead(R_barrierSensor); //Правый датчик.
  RC_free = digitalRead(RC_barrierSensor); //Центральный правый датчик.
  LC_free = digitalRead(LC_barrierSensor); //Центральный левый датчик.
  L_free = digitalRead(L_barrierSensor); //Левый датчик.
}

void move_forward() //Движение робота вперед.
{
  digitalWrite(M1, HIGH); //Левые колеса вращаются вперед.
  digitalWrite(M2, HIGH); //Правые колеса вращаются вперед.
  analogWrite(E1, motorSpeed); //Скорость вращения правых колес.
  analogWrite(E2, motorSpeed); //Скорость вращения левых колес.
}

void move_right() //Движение робота вправо.
{
  digitalWrite(M1, LOW); //Левые колеса вращаются назад.
  digitalWrite(M2, HIGH); //Правые колеса вращаются вперед.
  analogWrite(E1, motorSpeed); //Скорость вращения левых колес.
  analogWrite(E2, motorSpeed); //Скорость вращения правых колес.
}

void move_left() //Движение робота влево.
{
  digitalWrite(M1, HIGH); //Левые колеса вращаются вперед.
  digitalWrite(M2, LOW); //Правые колеса вращаются назад.
  analogWrite(E1, motorSpeed); //Скорость вращения левых колес.
  analogWrite(E2, motorSpeed); //Скорость вращения правых колес.
}

void move_stop() //Остановка робота.
{
  digitalWrite(M1, LOW); //Левые колеса вращаются назад.
  digitalWrite(M2, LOW); //Правые колеса вращаются назад.
  analogWrite(E1, 0); //Скорость вращения колес снизить до 0.
  analogWrite(E2, 0); //Скорость вращения колес снизить до 0.
}

void move_back() //Движение робота назад.
{
  digitalWrite(M1, LOW); //Левые колеса вращаются назад.
  digitalWrite(M2, LOW); //Правые колеса вращаются назад.
  analogWrite(E1, motorSpeed); //Скорость вращения левых колес.
  analogWrite(E2, motorSpeed); //Скорость вращения правых колес.
}

void setup() //Задание начальных параметров системы.
{
  irreceiver.enableIRIn(); //Инициализация ИК-приемника.
  motor_drive_config(); //Инициализация выводов платы расширения Motor Drive Shield.
  move_stop(); //Начальная позиция робота — робот стоит.
  //Serial.begin(9600);      // open the serial port at 9600 bps:
}

bool checkForSwitchingMode() {
  //Serial.print(results.value);
  if (results.value == 16769565 || results.value == 16753245) { // menu btn or power btn
    mode = menu;
    move_stop();
    return false;
  } else if (mode == menu) {
    if (results.value == 16724175) { // 1 btn
      mode = remote;
      move_stop();
      return false;
    } else if (results.value == 16718055) { // 2 btn
      mode = selfRun;
      move_stop();
      return false;
    } else if (results.value == 16743045) { // 3 btn
      mode = programEdit;
      move_stop();
      return false;
    }
  } else {
    return true;
  }
}

bool loopRemote() { // цикл ручного управления с пульта
  if (results.value == 16712445) { //Если значение равно 16712445…
    move_forward(); //Движение вперед.
    return false;
  }
  else if (results.value == 16750695) { //Или, если значение равно 16750695…
    move_back(); //Движение назад.
    return false;
  }
  else if (results.value == 16769055) { //Или, если значение равно 16769055…
    move_left(); //Поворот влево.
    return false;
  }
  else if (results.value == 16748655) { //Или, если значение равно 16748655…
    move_right(); //Поворот вправо.
    return false;
  }
  else if (results.value == 16754775) { //Или, если значение равно 16754775…
    move_stop(); //Остановка.
    return false;
  } else {
    return true;
  }
}

void loopProgramEdit() { // цикл программирования движения

}

void loopProgramRun()
{

}

void loopSelfRun()
{
  move_forward();

  scan_barrier_sensors();

  if (R_free && RC_free && LC_free && L_free) { //Путь впереди свободен.
    move_forward();
  }
  else if (R_free || L_free) {
    if (R_free) move_right();
    else if (L_free) move_left();
  }
  else { //Робот в тупике, ехать некуда, будем выбираться.
    move_back(); //Робот немного отъезжает назад.
    delay(500);
    move_stop(); //Остановка робота.
    delay(500);
    tryes++; //Увеличить количество попыток выхода из тупика.

    if (tryes > 3) { //Если попыток выхода из тупика больше трех…
      tryes = 0; //Новый отсчет количества попыток, так как меняем направление.
      movingRight = !movingRight; //Смена направления движения робота.
    }

    //Поворот робота в сторону.
    if (movingRight) move_right();
    else move_left();

    delay(500);
    move_stop(); //Остановка.
    delay(500);
  }
}

void loop() //Основной цикл
{
  if (irreceiver.decode(&results)) { //Если приемник получил сигнал…
    bool needToProcess = checkForSwitchingMode();
    if (mode == remote && needToProcess == true) {
      needToProcess = loopRemote();
    }
    irreceiver.resume(); //Возвращаем ИК-приемник в режим ожидания.
  }

  if (mode == selfRun) {
    //Serial.print("selfRun");
    loopSelfRun();
  } else if (mode == programEdit) {
    loopProgramEdit();
  } else if (mode == programRun) {
    loopProgramRun();
  }
}
