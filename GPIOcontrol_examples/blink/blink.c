    #include <wiringPi.h>
    #include <stdbool.h>

int main(void)
{
  bool toggle = 0;
  wiringPiSetup () ;
  pinMode (1, OUTPUT) ;

  while(1){
  toggle = !toggle;
  digitalWrite(1, toggle);
  delay(500);
  }
  return 0;
}

