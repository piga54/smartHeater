    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <wiringPi.h>
    #include <stdbool.h>
    #define  BUFSIZE  128

int main(void)
{
  float temp;
  int fd;
  int ret;
  int i,j;
  char buf[BUFSIZE];
  char tempBuf[5];
  bool toggle = 0;

  wiringPiSetup () ;
  pinMode (1, OUTPUT) ;

  while(1){
  fd = open("/sys/bus/w1/devices/28-001522141bee/w1_slave", O_RDONLY);

  if(-1 == fd){
    perror("open device file error");
    return 1;
  }


  while(1){
    ret = read(fd, buf, BUFSIZE);
    if(0 == ret){
      break;
    }
    if(-1 == ret){
      if(errno == EINTR){
        continue;
      }
      perror("read()");
      close(fd);
      return 1;
    }
  }

  for(i=0;i<sizeof(buf);i++){
    if(buf[i] == 116){    //instead of 116 it was 't'
      for(j=0;j<sizeof(tempBuf);j++){
        tempBuf[j] = buf[i+2+j];

      }
    }
  }

  temp = (float)atoi(tempBuf) / 1000;

printf("%.3f C\n",temp);
toggle = !toggle;

//blinking
digitalWrite(1, toggle);

  close(fd);
}
  return 0;
}
