        #include <QApplication>
	#include <QTextEdit>

#include <stdio.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <unistd.h>

#include <errno.h>

#include <stdlib.h>

#include <stdio.h>


#define BUFSIZE 128


        int main(int argv, char **args)
        {

double  temp;

//int i, j;

int fd;

int ret;

//int sentinel=0;


char buf[BUFSIZE];

char tempBuf[5];

            QApplication app(argv, args);

            QTextEdit textEdit;
            textEdit.show();

fd = open("/sys/bus/w1/devices/28-0015230752ee/w1_slave", O_RDONLY);

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


for(std::size_t i=0;i<sizeof(buf);i++){

if(buf[i] == 116){ //instead of 116 it was 't'

for(std::size_t j=0;j<sizeof(tempBuf);j++){

tempBuf[j] = buf[i+2+j];

}

}

}


temp = (double)atoi(tempBuf) / 1000;

//printf("%.3f C\n",temp);

close(fd);

//return 0;

	//float a=1.51;
	QString s = QString::number(temp);
	textEdit.setText(s);

            return app.exec();
        }
