#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include "Nextion.h"
#include "Wire.h"
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include "TSL2561.h"
TSL2561 tsl(TSL2561_ADDR_FLOAT);
#define FS_NO_GLOBALS
#include <FS.h>
#define WIFI_STA_PASS "1234567890"

// Fully Version for sale  single LED
ESP8266WebServer server(80);   //Web server object. Will be listening in port 80 (default for HTTP)
const char* ssid = "MiniSpec63109";
int led = 0;
File myFile;
File root;
const char* htmlfile = "/index.html";
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;

uint16_t last = 0;
byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;


float s_red = 0, s_green = 0, s_blue = 0 , s_blank = 0;
float abs_red = 0, abs_green = 0, abs_blue = 0;
float b_red = 0, b_green = 0, b_blue = 0;
float Slope = 0;
float Inter = 0;
int sd_sta = 0;
float abs_ = 0;
#define red_led  D0   //D4
#define green_led  D4   //D0
#define blue_led  D3   //D3

unsigned long time_1;
unsigned long time_ = 0;

byte cal_senac = 0; // active to sent datato calpage


String sd_name = "";

String las_page = "";
byte page_active = 0;

float con_data[5];
float abs_data[5];
char cal_colo[3] = {0, 0, 0};
int std_loca = 0;

float tim_kin = 0;

byte setting_resolution = 0;

byte led_typ = 0;

bool ledActive_Red = true;
bool ledActive_Green = true;
bool ledActive_Blue = true;

NexPage p_manu    = NexPage(0, 0, "manu_");
NexPage p_abs    = NexPage(1, 0, "abs_");
NexPage p_cal    = NexPage(2, 0, "cal_");
NexPage p_con    = NexPage(3, 0, "con_");
NexPage p_kin    = NexPage(4, 0, "kin_");
NexPage p_sou    = NexPage(5, 0, "sou_");
NexPage p_tab    = NexPage(6, 0, "table_");
NexPage p_num    = NexPage(7, 0, "num_");
NexPage p_text    = NexPage(8, 0, "Text");
NexPage p_set    = NexPage(9, 0, "setting");


/*

   setting manu
*/

NexCheckbox Typ_1 = NexCheckbox(9, 4, "c0");
NexCheckbox Typ_2 = NexCheckbox(9, 5, "c1");

NexCheckbox sen_1 = NexCheckbox(9, 6, "c2");
NexCheckbox sen_2 = NexCheckbox(9, 7, "c3");
NexCheckbox sen_3 = NexCheckbox(9, 8, "c4");

NexText tex_typ = NexText(9, 2, "g2");

NexText sen_typ = NexText(9, 3, "g3");
NexButton reset_name = NexButton(9, 18, "b1");



/*
   page manu
*/

NexButton manu_abs = NexButton(0, 1, "b0");
NexButton manu_con = NexButton(0, 2, "b1");
NexButton manu_cal = NexButton(0, 3, "b2");
NexButton manu_kin = NexButton(0, 4, "b3");
NexButton manu_sou = NexButton(0, 5, "b4");
NexButton manu_set = NexButton(0, 6, "b5");

//NexText manu_date = NexText(0, 8, "g0");
NexText manu_time = NexText(0, 8, "g0");

char buffer[10] = {0};


/*
   exit button
*/

NexButton exit_abs = NexButton(1, 7, "b5");
NexButton exit_con = NexButton(3, 9, "b5");
NexButton exit_cal = NexButton(2, 17, "b5");
NexButton exit_kin = NexButton(4, 2, "b5");
NexButton exit_sou = NexButton(5, 5, "b5");
NexButton exit_set = NexButton(9, 1, "b0");


/*
   Register a button object to the touch event list.
*/


/*
   absortion page
*/


NexButton abs_bla = NexButton(1, 5, "b3");
NexButton abs_sam = NexButton(1, 6, "b4");
NexButton abs_r = NexButton(1, 2, "b0");
NexButton abs_g = NexButton(1, 3, "b1");
NexButton abs_b = NexButton(1, 4, "b2");

/*
   calibration
*/

NexRadio cal_red = NexRadio(2, 2, "r0");
NexRadio cal_green = NexRadio(2, 3, "r1");
NexRadio cal_blue = NexRadio(2, 4, "r2");

NexText cal_c1 = NexText(2, 7, "t3");
NexText cal_c2 = NexText(2, 9, "t5");
NexText cal_c3 = NexText(2, 11, "t7");
NexText cal_c4 = NexText(2, 13, "t9");
NexText cal_c5 = NexText(2, 19, "t12");

NexText cal_a1 = NexText(2, 8, "t4");
NexText cal_a2 = NexText(2, 10, "t6");
NexText cal_a3 = NexText(2, 12, "t8");
NexText cal_a4 = NexText(2, 14, "t10");
NexText cal_a5 = NexText(2, 20, "t13");

NexButton cal_bla = NexButton(2, 15, "b3");
NexButton cal_sam = NexButton(2, 16, "b4");
NexButton cal_cal = NexButton(2, 21, "b0");
NexButton cal_save = NexButton(2, 22, "b1");

NexButton cal_lin = NexButton(2, 18, "t11");


/*
   page number
*/

NexText num_label = NexText(7, 2, "t0");
NexText num_data = NexText(7, 3, "t1");

NexButton num_ent = NexButton(7, 14, "t11");


/*


   page table
*/
NexButton tab_la = NexButton(6, 1, "b0");
NexButton tab_la1 = NexButton(6, 2, "b1");
NexButton tab_la2 = NexButton(6, 3, "b2");

NexButton tab_n1 = NexButton(6, 4, "b3");
NexButton tab_n2 = NexButton(6, 7, "b6");
NexButton tab_n3 = NexButton(6, 10, "b9");
NexButton tab_n4 = NexButton(6, 13, "b12");
NexButton tab_n5 = NexButton(6, 16, "b15");
NexButton tab_n6 = NexButton(6, 19, "b18");
NexButton tab_n7 = NexButton(6, 22, "b21");

NexButton tab_ab1 = NexButton(6, 5, "b4");
NexButton tab_ab2 = NexButton(6, 8, "b7");
NexButton tab_ab3 = NexButton(6, 11, "b10");
NexButton tab_ab4 = NexButton(6, 14, "b13");
NexButton tab_ab5 = NexButton(6, 17, "b16");
NexButton tab_ab6 = NexButton(6, 20, "b19");
NexButton tab_ab7 = NexButton(6, 23, "b22");

NexButton tab_co1 = NexButton(6, 6, "b5");
NexButton tab_co2 = NexButton(6, 9, "b8");
NexButton tab_co3 = NexButton(6, 12, "b11");
NexButton tab_co4 = NexButton(6, 15, "b14");
NexButton tab_co5 = NexButton(6, 18, "b17");
NexButton tab_co6 = NexButton(6, 21, "b20");
NexButton tab_co7 = NexButton(6, 24, "b23");

NexButton tab_for = NexButton(6, 25, "b24");
NexButton tab_rev = NexButton(6, 26, "b25");
NexButton tab_ent = NexButton(6, 27, "b26");
NexButton tab_exi = NexButton(6, 28, "b27");

NexText text_show = NexText(8, 1, "t0");
NexButton text_ent = NexButton(8, 39, "b42");
NexButton text_exi = NexButton(8, 40, "b43");

/*

   concentration
*/

NexRadio con_red = NexRadio(3, 2, "r0");
NexRadio con_green = NexRadio(3, 3, "r1");
NexRadio con_blue = NexRadio(3, 4, "r2");

NexButton con_lin = NexButton(3, 13, "b1");
NexButton con_bla = NexButton(3, 7, "b3");
NexButton con_save = NexButton(3, 8, "b4");
NexButton con_tab = NexButton(3, 10, "b0");

NexButton con_abs = NexButton(3, 11, "t3");
NexButton con_con = NexButton(3, 12, "t4");
NexText con_lab = NexText(3, 1, "t0");


/*

   kinetic   page
*/

NexWaveform s0 = NexWaveform(4, 1, "s0");
static uint8_t ch0_data = 0;
static uint8_t ch1_data = 0;
static uint8_t ch2_data = 0;
static uint8_t ch3_data = 0;
NexButton kin_bla = NexButton(4, 3, "b0");
NexButton kin_sta = NexButton(4, 4, "b1");
NexButton kin_tim = NexButton(4, 5, "b2");
NexText kin_r = NexText(4, 6, "t0");
NexText kin_g = NexText(4, 7, "t1");
NexText kin_b = NexText(4, 8, "t2");
NexText kin_lable = NexText(4, 9, "t3");
int kin_ = 0;
long kin_timeRevo = 0;

/*

   source page
*/

NexButton sou_r = NexButton(5, 2, "b0");
NexButton sou_g = NexButton(5, 3, "b1");
NexButton sou_b = NexButton(5, 4, "b2");

NexText sou_sta = NexText(5, 6, "t1");



NexTouch *nex_listen_list[] =
{
  &abs_bla,
  &abs_sam,
  &abs_r,
  &abs_g,
  &abs_b,

  &manu_abs,
  &manu_con,
  &manu_cal,
  &manu_kin,
  &manu_sou,
  &manu_set,

 // &manu_date,
  &manu_time,

  &exit_abs,
  &exit_con,
  &exit_cal,
  &exit_kin,
  &exit_sou,

  &cal_red,
  &cal_green,
  &cal_blue,

  &cal_c1,
  &cal_c2,
  &cal_c3,
  &cal_c4,
  &cal_c5,
  &cal_bla,
  &cal_sam,
  &cal_cal,
  &cal_save,

  &num_ent,

  &tab_for,
  &tab_rev,
  &tab_ent,
  &tab_exi,
  &tab_n1,
  &tab_n2,
  &tab_n3,
  &tab_n4,
  &tab_n5,
  &tab_n6,
  &tab_n7,

  &text_ent,
  &text_exi,

  &con_lin,
  &con_bla,
  &con_save,
  &con_tab,

  &kin_bla,
  &kin_sta,
  &kin_tim,

  &Typ_1,
  &Typ_2,
  &sen_1,
  &sen_2,
  &sen_3,
  &reset_name,
  &exit_set,


  NULL
};

/*
   Button component pop callback function.
   In this example,the button's text value will plus one every time when it is released.
*/



void manu_absCallback(void *ptr)
{
  bee(10);
  page_active = 1;
  p_abs.show();

}
void manu_conCallback(void *ptr)
{
  bee(10);
  page_active = 3;
  p_con.show();
  cal_colo[0] = 0;
  cal_colo[1] = 0;
  cal_colo[2] = 0;
  con_red.setValue(cal_colo[0]);
  con_green.setValue(cal_colo[1]);
  con_blue.setValue(cal_colo[2]);


  Slope = 0;
  Inter = 0;

}
void manu_calCallback(void *ptr)
{
  bee(10);
  page_active = 2;
  p_cal.show();
  cal_colo[0] = 0;
  cal_colo[1] = 0;
  cal_colo[2] = 0;
  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  con_data[0] = 0;
  con_data[1] = 0;
  con_data[2] = 0;
  con_data[3] = 0;
  con_data[4] = 0;
  abs_data[0] = 0;
  abs_data[1] = 0;
  abs_data[2] = 0;
  abs_data[3] = 0;
  abs_data[4] = 0;

}
void manu_kinCallback(void *ptr)
{
  bee(10);

  bee(10);
  p_text.show();
  bee(100);
  text_show.setText("File name");
  delay(800);
  bee(100);
  text_show.setText("         ");
  text_show.setText("");
  las_page = "manu_kin";
  page_active = 8;


}
void manu_souCallback(void *ptr)
{
  bee(10);
  page_active = 5;
  p_sou.show();

}
void manu_setCallback(void *ptr)
{
  bee(10);
  page_active = 9;
  p_set.show();

}
/*
void manu_dateCallback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  num_label.setText("D:");
  num_data.setText("");
  las_page = "manu_date";

}
*/
void manu_timeCallback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  num_label.setText("H:");
  num_data.setText("");
  las_page = "manu_time";

}

void exit_absCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();

}
void exit_conCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();

}
void exit_calCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();

}
void exit_kinCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();

}
void exit_souCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();

}

void abs_blaCallback(void *ptr)
{
  bee(10);
  blank();

  abs_r.setText("0.00");
  abs_g.setText("0.00");
  abs_b.setText("0.00");

}
void abs_samCallback(void *ptr)
{
  bee(10);

  //sample_data();
  led =0;
  /*
    s_red = 1000;
    s_green = 900;
    s_blue = random(200,500);
  */
}

void cal_redCallback(void *ptr)
{
  bee(10);
  if (cal_colo[0] == 0)
  {
    cal_colo[0]  = 1;
  } else
  {
    cal_colo[0]  = 0;
  }
  cal_colo[1]  = 0;
  cal_colo[2]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();
  cal_senac = 1;

}

void cal_redweb()
{
  if (page_active != 2)
  {
    p_cal.show();
  }
  page_active = 2;

  bee(10);
  if (cal_colo[0] == 0)
  {
    cal_colo[0]  = 1;
  } else
  {
    cal_colo[0]  = 0;
  }
  cal_colo[1]  = 0;
  cal_colo[2]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();
 


  server.send(200, "text/plain", "read");
   cal_senac = 1;
  
}
void cal_greenCallback(void *ptr)
{
  bee(10);
  if (cal_colo[1] == 0)
  {
    cal_colo[1]  = 1;
  } else
  {
    cal_colo[1]  = 0;
  }
  cal_colo[0]  = 0;
  cal_colo[2]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();
  cal_senac = 1;
}

void cal_greenweb()
{
  if (page_active != 2)
  {
    p_cal.show();
  }
  page_active = 2;
  bee(10);
  if (cal_colo[1] == 0)
  {
    cal_colo[1]  = 1;
  } else
  {
    cal_colo[1]  = 0;
  }
  cal_colo[0]  = 0;
  cal_colo[2]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();
 
  server.send(200, "text/plain", "read");
  cal_senac = 1;
}
void cal_blueCallback(void *ptr)
{
  bee(10);
  if (cal_colo[2] == 0)
  {
    cal_colo[2]  = 1;
  } else
  {
    cal_colo[2]  = 0;
  }
  cal_colo[1]  = 0;
  cal_colo[0]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();
  cal_senac = 1;
}

void cal_blueweb()
{
  if (page_active != 2)
  {
    p_cal.show();
  }
  page_active = 2;
  bee(10);
  if (cal_colo[2] == 0)
  {
    cal_colo[2]  = 1;
  } else
  {
    cal_colo[2]  = 0;
  }
  cal_colo[1]  = 0;
  cal_colo[0]  = 0;

  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  bc_set();

  server.send(200, "text/plain", "read");
  cal_senac = 1;
}

void setcaltable()
{
  String id_table = server.arg("id_num"); //Refer  xhttp.open("GET", "setLED?LEDstate="+led, true);
  String con = server.arg("Val");
  p_cal.show();
  con_data[id_table.toInt() - 1] = con.toFloat();
  updatecal_c();
  cal_senac = id_table.toInt();
  page_active = 2;
  bc_set();
  server.send(200, "text/plain", "read");
  if (id_table.toInt() >= 5)
  {
    cal_senac = 0;
  } else
  {
    cal_senac = id_table.toInt() + 1;
  }



}
void cal_c1Callback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  las_page = "cal_c1";
  num_label.setText("C1");
  num_data.setText("");
  cal_senac = 2;
}
void cal_c2Callback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  las_page = "cal_c2";
  num_label.setText("C2");
  num_data.setText("");
  cal_senac = 3;
}
void cal_c3Callback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  las_page = "cal_c3";
  num_label.setText("C3");
  num_data.setText("");
  cal_senac = 4;
}
void cal_c4Callback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  las_page = "cal_c4";
  num_label.setText("C4");
  num_data.setText("");
  cal_senac = 5;

}
void cal_c5Callback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  las_page = "cal_c5";
  num_label.setText("C5");
  num_data.setText("");
  cal_senac = 0;
}


void cal_blaCallback(void *ptr)
{
  bee(10);
  blank();


}
void cal_samCallback(void *ptr)
{
  bee(10);
  cal_senac = 0;
}

void calcaliweb()
{
  bee(10);
  float concen[5] = {0};
  float absor[5] = {0};
  for (int i = 1; i < 5; i++)
  {
    concen[i] = con_data[i];
    absor[i] = abs_data[i];
  }

  int a = 1;
  for (int i = 1; i < 5; i++)
  {
    if (concen[i] > 0.0)
    {
      a = i;
    }
  }
  //Serial.print(a);
  linearOut(concen, absor, a + 1);
  char buffer1[20];
  String showT = "y = " + String(Slope, 3) + "x ";
  if (Inter == abs(Inter))
  {
    showT += "+ ";
  }
  showT += String(Inter, 3);
  memset(buffer1, 0, sizeof(buffer1));
  showT.toCharArray(buffer1, sizeof(buffer1));

  cal_lin.setText(buffer1);
  delay(10);
  server.send(200, "text/plain", showT);
}
void cal_calCallback(void *ptr)
{
  bee(10);
  float concen[5] = {0};
  float absor[5] = {0};

  memset(buffer, 0, sizeof(buffer));
  cal_c1.getText(buffer, sizeof(buffer));
  concen[0] = atof(buffer);
  //Serial.print(concen[0]);//Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_c2.getText(buffer, sizeof(buffer));
  concen[1] = atof(buffer);
  //Serial.print(concen[1]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_c3.getText(buffer, sizeof(buffer));
  concen[2] = atof(buffer);
  // Serial.print(concen[2]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_c4.getText(buffer, sizeof(buffer));
  concen[3] = atof(buffer);
  //Serial.print(concen[1]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_c5.getText(buffer, sizeof(buffer));
  concen[4] = atof(buffer);
  // Serial.print(concen[2]);Serial.print("  ");


  memset(buffer, 0, sizeof(buffer));
  cal_a1.getText(buffer, sizeof(buffer));
  absor[0] = atof(buffer);
  //Serial.print(absor[0]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_a2.getText(buffer, sizeof(buffer));
  absor[1] = atof(buffer);
  //Serial.print(absor[1]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_a3.getText(buffer, sizeof(buffer));
  absor[2] = atof(buffer);
  memset(buffer, 0, sizeof(buffer));
  cal_a4.getText(buffer, sizeof(buffer));
  absor[3] = atof(buffer);
  //Serial.print(absor[1]);Serial.print("  ");
  memset(buffer, 0, sizeof(buffer));
  cal_a5.getText(buffer, sizeof(buffer));
  absor[4] = atof(buffer);
  // Serial.print(absor[2]);Serial.println("  ");
  int a = 1;
  for (int i = 1; i < 5; i++)
  {
    if (concen[i] > 0.0)
    {
      a = i;
    }
  }
  //Serial.print(a);
  linearOut(concen, absor, a + 1);
  char buffer1[20];
  String showT = "y = " + String(Slope, 3) + "x ";
  if (Inter == abs(Inter))
  {
    showT += "+ ";
  }
  showT +=  String(Inter, 3);
  memset(buffer1, 0, sizeof(buffer1));
  showT.toCharArray(buffer1, sizeof(buffer1));

  cal_lin.setText(buffer1);
}


void cal_saveCallback(void *ptr)
{
  bee(10);
  p_tab.show();
  page_active = 6;
  las_page = "cal_save";
  gotable1(0);
  memset(buffer, 0, sizeof(buffer));
  tab_for.setText(">>");
  tab_rev.setText(buffer);


}

void num_entCallback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  num_data.getText(buffer, sizeof(buffer));

  if (las_page == "cal_c1")
  {
    p_cal.show();
    con_data[0] = atof(buffer);
    updatecal_c();
    cal_senac = 2;
    page_active = 2;
    bc_set();
  } else if (las_page == "cal_c2")
  {
    p_cal.show();

    con_data[1] = atof(buffer);
    updatecal_c();
    page_active = 2;
    bc_set();
  }
  else if (las_page == "cal_c3")
  {
    p_cal.show();

    con_data[2] = atof(buffer);
    updatecal_c();
    page_active = 2;
    bc_set();
  }
  else if (las_page == "cal_c4")
  {
    p_cal.show();

    con_data[3] = atof(buffer);
    updatecal_c();
    page_active = 2;
    bc_set();
  } else if (las_page == "cal_c5")
  {
    p_cal.show();

    con_data[4] = atof(buffer);
    updatecal_c();
    page_active = 2;
    bc_set();
  } else if (las_page == "kin_tim")
  {
    tim_kin = atof(buffer);

    doubleTorom(400,tim_kin);
    page_active = 4;
    p_kin.show();
  }
  else if (las_page == "manu_time1")
  {
    bee(10);
    int dateset = atoi(buffer);
    if (dateset <= 60)
    {
      minute = dateset;
      second = 0;
      las_page = "manu_time1";

      page_active = 0;
      p_manu.show();
    } else
    {
      las_page = "manu_time";
      bee(20);
      delay(100);
      bee(20);

    }
  }



}

void tab_forCallback(void *ptr)
{
  bee(10);
  gotable1(7);
  memset(buffer, 0, sizeof(buffer));
  tab_for.setText(buffer);
  tab_rev.setText("<<");


}
void tab_revCallback(void *ptr)
{
  bee(10);
  gotable1(0);
  memset(buffer, 0, sizeof(buffer));
  tab_for.setText(">>");
  tab_rev.setText(buffer);

}
void tab_entCallback(void *ptr)
{
  bee(10);
  p_cal.show();
  updatecal_c();
  page_active = 2;
  bc_set();

}
void tab_exiCallback(void *ptr)
{
  bee(10);
  page_active = 0;
  p_manu.show();
}
void tab_n1Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 0;
  } else
  {
    std_loca = 7;
  }

  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);

  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }

}
void tab_n2Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 1;
  } else
  {
    std_loca = 8;
  }
  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));
    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}
void tab_n3Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 2;
  } else
  {
    std_loca = 9;
  }
  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}
void tab_n4Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 3;
  } else
  {
    std_loca = 10;
  }
  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}
void tab_n5Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 4;
  } else
  {
    std_loca = 11;
  }
  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}
void tab_n6Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 5;
  } else
  {
    std_loca = 12;
  }
  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}
void tab_n7Callback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  tab_for.getText(buffer, sizeof(buffer));
  if (buffer[0] == '>')
  {
    std_loca = 6;
  } else
  {
    std_loca = 13;
  }

  if (las_page == "con_lin")
  {
    Slope = std_m(std_loca);
    Inter = std_c(std_loca);
    read_stdled(std_loca);
    page_active = 3;
    p_con.show();
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y = ";
    data += String(Slope, 3);
    data += "x ";
    if (Inter == abs(Inter))
    {
      data += "+ ";
    }
    data += String(Inter, 3);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
  } else
  {
    las_page = "tab_n";
    page_active = 8;

    p_text.show();
  }
}

void text_entCallback(void *ptr)
{
  bee(10);
  memset(buffer, 0, sizeof(buffer));
  text_show.getText(buffer, sizeof(buffer));
  if (las_page == "tab_n")
  {
    write_std(std_loca, buffer, Slope, Inter);
    p_cal.show();
    page_active = 2;
  } else if (las_page == "con_save")
  {
    p_con.show();
    sd_name = "";
    page_active = 3;
    for (int i = 0 ; i < 10; i++)
    {
      if (buffer[i] != 0)
      {
        sd_name = sd_name + buffer[i];
      }

    }
    sd_name = sd_name + ".csv";

    //    filename = filename


    sd_save(sd_name, buffer);
    memset(buffer, 0, sizeof(buffer));
    sd_name.toCharArray(buffer, sizeof(buffer));
    con_lab.setText(buffer);

  } else if (las_page == "con_save1")
  {
    p_con.show();

    page_active = 3;
    sd_save(sd_name, buffer);
    memset(buffer, 0, sizeof(buffer));
    sd_name.toCharArray(buffer, sizeof(buffer));
    con_lab.setText(buffer);

  } else if (las_page == "manu_kin")
  {
    p_kin.show();
    sd_name = "";
    page_active = 4;
    for (int i = 0 ; i < 10; i++)
    {
      if (buffer[i] != 0)
      {
        sd_name = sd_name + buffer[i];
      }

    }
    sd_name = sd_name + ".csv";


    kin_save(sd_name, 0, 0, 0, 0);
    //memset(buffer, 0, sizeof(buffer));
    //sd_name.toCharArray(buffer,sizeof(buffer));
    //con_lab.setText(buffer);

  }
  else
  {
    p_tab.show();
    page_active = 6;
    las_page = "";
    gotable1(0);
    memset(buffer, 0, sizeof(buffer));
    tab_for.setText(">>");
    tab_rev.setText(buffer);

  }


}
void text_exiCallback(void *ptr)
{
  bee(10);
  page_active = 0;   p_manu.show();
}



void con_linCallback(void *ptr)
{
  bee(10);
  p_tab.show();   page_active = 6;
  las_page = "con_lin";
  gotable1(0);
  memset(buffer, 0, sizeof(buffer));
  tab_for.setText(">>");
  tab_rev.setText(buffer);

}
void con_blaCallback(void *ptr)
{
  bee(10);
  blank();

}
void con_saveCallback(void *ptr)
{
  bee(10);
  if (sd_name.length() <= 2)
  {
    bee(10);
    p_text.show();
    bee(100);
    text_show.setText("File name");
    delay(800);
    bee(100);
    text_show.setText("         ");
    text_show.setText("");
    las_page = "con_save";
    page_active = 8;
  } else {

    p_text.show();
    bee(100);
    text_show.setText("name");
    delay(800);
    bee(100);
    text_show.setText("         ");
    text_show.setText("");
    las_page = "con_save1";
    page_active = 8;

  }

}
void con_tabCallback(void *ptr)    /// con name to set name
{

  bee(10);
  p_text.show();
  bee(100);
  text_show.setText("File name");
  delay(800);
  bee(100);
  text_show.setText("         ");
  text_show.setText("");
  las_page = "con_save";
  page_active = 8;
}
void kin_blaCallback(void *ptr)
{
  bee(10);
  blank();

}
void kin_staCallback(void *ptr)
{
  bee(10);
  if (kin_ == 0)
  {
    kin_ = 1;
    kin_sta.setText("Stop");

  } else
  {
    kin_ = 0;
    kin_sta.setText("Start");
  }
  kin_timeRevo = 0;

}
void kin_timCallback(void *ptr)
{
  bee(10);
  page_active = 7;
  p_num.show();
  num_label.setText("Time(s)");
  num_data.setText("");
  las_page = "kin_tim";
}
/////
void Typ_1Callback(void *ptr)
{
  bee(10);
  page_active = 9;
  EEPROM.write(410, 1);
  EEPROM.commit();

  tex_typ.setText("Save Typ1");
  led_typ = 1;
  Typ_2.setValue(0);
}
void Typ_2Callback(void *ptr)
{
  bee(10);
  page_active = 9;
  EEPROM.write(410, 2);
  EEPROM.commit();

  tex_typ.setText("Save Typ2");
  led_typ = 2;
  Typ_1.setValue(0);
}

void sen_1Callback(void *ptr)
{
  bee(10);
  page_active = 9;
  EEPROM.write(411, 1);
  EEPROM.commit();

  sen_typ.setText("Save Sen1");
  setting_resolution = 1;
  configureSensor();
  sen_2.setValue(0);
  sen_3.setValue(0);

}
void sen_2Callback(void *ptr)
{
  bee(10);
  page_active = 9;
  EEPROM.write(411, 2);
  EEPROM.commit();

  sen_typ.setText("Save Sen2");
  setting_resolution = 2;
  configureSensor();
  sen_1.setValue(0);
  sen_3.setValue(0);
}
void sen_3Callback(void *ptr)
{
  bee(10);
  page_active = 9;
  EEPROM.write(411, 3);
  EEPROM.commit();

  sen_typ.setText("Save Sen3");
  setting_resolution = 3;
  configureSensor();
  sen_2.setValue(0);
  sen_1.setValue(0);
}

void reset_nameCallback(void *ptr)
{
  /// reset name
  bee(10);

  bee(10);
  String emtry_name = "          ";
  memset(buffer, 0, sizeof(buffer));
  emtry_name.toCharArray(buffer, sizeof(buffer));
  for (int i = 0; i < 14; i++)
  {
    write_std(i, buffer, 0, 0);
  }


  bee(10);
  bee(10);
}



void exit_setCallback(void *ptr)
{
  bee(10);

  page_active = 0;
  p_manu.show();


}


void Absdata()
{
  if (page_active != 1)
  {
    p_abs.show();
  }
  page_active = 1;
  websample_read();

}


void gettablecal()
{

  String message = "";
  for (int i = 0; i < 5; i++)
  {
    message += String(con_data[i]);
    message += ",";
  }
  for (int i = 0; i < 5; i++)
  {
    message += String(abs_data[i]);
    message += ",";
  }
  server.send(200, "text/plain", message);

}

void savecali()
{
  String name_ = server.arg("id_nname");
  String caliname = "";
  if (name_.length() > 6)
  {
    caliname = name_.substring(0, 6);
  } else
  {
    caliname = name_;
  }
  int index = caliname.indexOf(".");
  String name_data = "";
  if (index >= 0 )
  {
    name_data = caliname.substring(0, index);
    name_data += ".csv";
  } else
  {
    name_data = caliname + ".csv";
  }


  int index_save = -1;
  for (int i = 0; i < 14; i++)
  {
    std_name(i);

    if (buffer[0] == ' ' && buffer[1] == ' ' && buffer[2] == ' ')
    {
      index_save = i;
      memset(buffer, 0, sizeof(buffer));
      name_data.toCharArray(buffer, sizeof(buffer));
      write_std(index_save, buffer, Slope, Inter);
      break;
    }
  }

  delay(10);
  if (index_save == -1)
  {
    server.send(200, "text/plain", "ERROR Full data");
  } else
  {
    server.send(200, "text/plain", "Save" + name_data);
  }

}

void readstdtable()
{
  String datasent = "";
  for (int i = 0; i < 14; i++)
  {
    memset(buffer, 0, sizeof(buffer));
    std_name(i);
    datasent += buffer;
    datasent += ",";
  }

  for (int i = 0; i < 14; i++)
  {
    datasent += String(std_m(i), 3);
    datasent += ",";
  }

  for (int i = 0; i < 14; i++)
  {
    datasent += String(std_c(i), 3);
    datasent += ",";


  }
  server.send(200, "text/plain", datasent);
}

void con()
{

  if (page_active != 3)
  {
    page_active = 3;
    p_con.show();
  }
  delay(1);
  String data_ = String(abs_data[0]) + "," + String(con_data[0]) + ",";
  data_ += "Y = ";
  data_ += String(Slope);
  data_ += " X ";
  if (abs(Inter) == Inter)
  {
    data_ += " + ";
  }
  data_ += String(Inter);

  server.send(200, "text/plain", data_);
}
void stdTable()
{
  String id_ = server.arg("id_");
  if (page_active != 3)
  {
    page_active = 3;
    p_con.show();
  }
  std_loca = id_.toInt();
  Slope = std_m(std_loca);
  Inter = std_c(std_loca);
  int led_ = read_stdled(std_loca);
  page_active = 3;
  p_con.show();

  delay(1);
  con_red.setValue(cal_colo[0]);
  con_green.setValue(cal_colo[1]);
  con_blue.setValue(cal_colo[2]);
  String data = "y=";
  data += String(Slope, 2);
  data += "x";
  if (Inter == abs(Inter))
  {
    data += "+";
  }
  data += String(Inter, 2);
  char buffer1[20] = {0};
  data.toCharArray(buffer1, sizeof(buffer1));

  con_lin.setText(buffer1);

  server.send(200, "text/plain", String(led_));
}

void saveconcen()
{
  String file_name = server.arg("filename");
  String data_save = server.arg("data");


  //String name_ = server.arg("id_nname");
  String namesave = "";
  if (file_name.length() > 6)
  {
    namesave = file_name.substring(0, 6);
  } else
  {
    namesave = file_name;
  }
  int index = namesave.indexOf(".");
  String name_data = "";
  if (index >= 0 )
  {
    name_data = namesave.substring(0, index);
    name_data += ".csv";
  } else
  {
    name_data = namesave + ".csv";
  }

  p_con.show();
  sd_name = "";
  page_active = 3;



  sd_save(name_data, data_save);
  memset(buffer, 0, sizeof(buffer));
  file_name.toCharArray(buffer, sizeof(buffer));
  con_lab.setText(buffer);
  server.send(200, "text/plain", "save" + name_data);
}

void stdDel()
{
  String id_ = server.arg("id_");
  std_loca = id_.toInt();
  String name_data = "          ";
  memset(buffer, 0, sizeof(buffer));
  name_data.toCharArray(buffer, sizeof(buffer));
  write_std(std_loca, buffer, 0, 0);
  server.send(200, "text/plain", "save");
}

void start_kin()
{
  String file_name = server.arg("file_name");
  String time_loop = server.arg("time");

   
  String caliname = "";
  if (file_name.length() > 6)
  {
    caliname = file_name.substring(0, 6);
  } else
  {
    caliname = file_name;
  }
  int index = caliname.indexOf(".");
  String name_data = "";
  if (index >= 0 )
  {
    name_data = caliname.substring(0, index);
    name_data += ".csv";
  } else
  {
    name_data = caliname + ".csv";
  }


  sd_name = name_data;
  tim_kin = time_loop.toInt();
 
    if (kin_ == 0)
  {
    kin_ = 1;
    kin_sta.setText("Stop");
    server.send(200, "text/plain", "Stop");

  } else
  {
    kin_ = 0;
    kin_sta.setText("Start");
    server.send(200, "text/plain", "Start");
  }

  
  kin_timeRevo = 0;
   p_kin.show();
    
    page_active = 4;
   
  //  sd_name = sd_name + ".csv";


   // kin_save(sd_name, 0, 0, 0, 0);
    
}

void export_data()
{
    String file_name = server.arg("file_name");

  String caliname = "";
  if (file_name.length() > 6)
  {
    caliname = file_name.substring(0, 6);
  } else
  {
    caliname = file_name;
  }
  int index = caliname.indexOf(".");
  String name_data = "";
  if (index >= 0 )
  {
    name_data = caliname.substring(0, index);
    name_data += ".csv";
  } else
  {
    name_data = caliname + ".csv";
  }


  SD_file_download(name_data);
  
}
void abs_kin()
{
 String  message = String(abs_data[0], 3) + "," + String(abs_data[1], 3) + "," + String(abs_data[2], 3);
  server.send(200, "text/plain", message);
 
}

void source()
{
    
  if(page_active != 5)
  {
   bee(10);
  page_active = 5;
  p_sou.show();
  }
    
    
   blank();
   
    /*
      b_red = random(750,900);
      b_green = random(750,900);
      b_blue = random(750,900);
    */


 String message = String(b_red) + "," + String(b_green) + "," + String(b_blue);
  server.send(200, "text/plain", message);
  

}


void setup() {
  Serial.begin(9600);
  EEPROM.begin(512); 
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  

  led_typ = EEPROM.read(410);
  setting_resolution = 2;//EEPROM.read(411);

 
  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
   // while (1);
  }
  //displaySensorDetails();
  configureSensor();
 if (led_typ == 1)
  {
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, HIGH);
    digitalWrite(blue_led, HIGH);
    int data = sensor_read();
    if(data > 100)
    {
      led_typ =2;
      Serial.println(" sensor typ 2");
    }
  } else
  {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, LOW);
     int data = sensor_read();
    if(data > 100)
    {
      led_typ =1;
      Serial.println(" sensor typ 1");
    }
  }

  if (led_typ == 1)
  {
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, HIGH);
    digitalWrite(blue_led, HIGH);
 Serial.println(" sensor typ 1");
  } else
  {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, LOW);
   Serial.println(" sensor typ 2");
  }

  // put your setup code here, to run once:
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, WIFI_STA_PASS);
  Wire.begin();
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);

  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "www.minispec.com", apIP);

  // simple HTTP server to see that DNS server is working
  Serial.begin(9600);
  Serial.println();

  //Initialize File System
  SPIFFS.begin();
 // Serial.println("File System Initialized");



  server.begin();
  server.on("/", handleRoot);

  server.on("/liner", liner_data);
  server.on("/connect", version_data);
  server.on("/data", data_read);
  server.on("/blank", set_blank);
  server.on("/signal", signal_data);
  server.onNotFound(handleWebRequests); //Set setver all paths are not found so we can handle as per URI

  server.on("/abs", Absdata);
  server.on("/cal_redweb", cal_redweb);
  server.on("/cal_greenweb", cal_greenweb);
  server.on("/cal_blueweb", cal_blueweb);
  server.on("/setcaltable", setcaltable);
  server.on("/gettablecal", gettablecal);
  server.on("/calcaliweb", calcaliweb);
  server.on("/savecali", savecali);
  server.on("/readstdtable", readstdtable);
  server.on("/con", con);
  server.on("/stdTable", stdTable);
  server.on("/saveconcen", saveconcen);
  server.on("/stdDel", stdDel);

  server.on("/start_kin", start_kin);
server.on("/abs_kin", abs_kin);
server.on("/export",export_data);
server.on("/source",source);
  server.begin();                                       //Start the server
  //  Serial.println("Server listening");
  IPAddress myIP = WiFi.softAPIP();
  // Serial.print("AP IP address: ");
  // Serial.println(myIP);

  






  time_1 = millis();
  time_ = millis();
  /*
    for(int i =0 ; i < 14; i++)
    {
    write_std(i,"-",i,i);

    }
  */
  bee(50);
  tim_kin = romTodouble(400);

  Serial.print("qr0.txt=");
  Serial.print('"');
  Serial.print("www.minispec.com");
  Serial.print('"');
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

}

void loop() {
  // put your main code here, to run repeatedly:
  //server.handleClient();
  dnsServer.processNextRequest();
  server.handleClient();
  nexLoop(nex_listen_list);
  if (millis() - time_1 > 1000 && page_active == 0)
  {

    time_1 = millis();
  }

  if (millis() - time_ > 600 && page_active == 1)
  {
    
   
    R_update(0);
    G_update(0);
    B_update(0);
    led =0;
    sample_data();
    
    time_ = millis();
    
  }
  else if (millis() - time_ > 1000 && page_active == 2)
  {
   

    led =0;
    R_update(cal_senac);
    G_update(cal_senac);
    B_update(cal_senac);
     led =0;
    sample_data();
    time_ = millis();
  }
  else if (millis() - time_ > 1000 && page_active == 3)
  {
    //sample_data();
     led =0;
    /*
      s_red = random(200,900);
      s_green = 900;
      s_blue = random(200,900);
    */
    R_update(cal_senac);
    G_update(cal_senac);
    B_update(cal_senac);
    con_red.setValue(cal_colo[0]);
    con_green.setValue(cal_colo[1]);
    con_blue.setValue(cal_colo[2]);
    String data = "y=";
    data += String(Slope, 2);
    data += "x";
    if (Inter == abs(Inter))
    {
      data += "+";
    }
    data += String(Inter, 2);
    char buffer1[20] = {0};
    data.toCharArray(buffer1, sizeof(buffer1));

    con_lin.setText(buffer1);
    time_ = millis();

  } else if (millis() - time_ > (tim_kin * 1000.0) && page_active == 4)
  {
    //sample_data();
   
    kin_add();
 led =0;
    sample_data();
    time_ = millis();
  }
  else if (millis() - time_ > (1000.0) && page_active == 5)
  {
    blank();
    /*
      b_red = random(750,900);
      b_green = random(750,900);
      b_blue = random(750,900);
    */
    memset(buffer, 0, sizeof(buffer));
    itoa(b_red, buffer, sizeof(buffer));
    sou_r.setText(buffer);
    memset(buffer, 0, sizeof(buffer));
    itoa(b_green, buffer, sizeof(buffer));
    sou_g.setText(buffer);
    memset(buffer, 0, sizeof(buffer));
    itoa(b_blue, buffer, sizeof(buffer));
    sou_b.setText(buffer);
    if (b_red > 800 || b_green > 800 || b_blue > 800)
    {
      sou_sta.Set_background_color_bco(65535);
      sou_sta.setText("Ready");
    } else
    {
      sou_sta.Set_background_color_bco(63488);
      sou_sta.setText("Low power");
    }
    led =0;
    sample_data();
    time_ = millis();
  }

sample_data();

}
String stringTodata(String data_x, String data_y, int size_)
{
  String mat = "";
  int string_lengthx = data_x.length();
  int string_lengthy = data_y.length();
  char buffx[string_lengthx] ;
  char buffy[string_lengthy] ;
  int index_ = 0;
  float x_signal[size_];
  float y_signal[size_];
  data_x.toCharArray(buffx, string_lengthx);
  data_y.toCharArray(buffy, string_lengthy);
  for (int i = 0; i < string_lengthx; i++)
  {
    if (buffx[i] == ',')
    {
      // Serial.print("Xdata = ");
      x_signal[index_] = mat.toFloat();
      // Serial.println(x_signal[index_]);
      index_++;
      mat = "";
    } else if (buffx[i] == '"')
    {

    } else
    {
      mat = mat + buffx[i];
    }
  }
  index_ = 0;
  mat = "";
  for (int i = 0; i < string_lengthy; i++)
  {
    if (buffy[i] == ',')
    {
      y_signal[index_] = mat.toFloat();
      //Serial.print("Ydata = ");
      // Serial.println(y_signal[index_]);
      index_++;
      mat = "";
    } else if (buffy[i] == '"')
    {

    } else
    {
      mat = mat + buffy[i];
    }
  }

  return linearOut(x_signal, y_signal, size_);

}
void liner_data()
{ String message = "";
  String x_data = "";
  String y_data = "";
  int size_data = 0;

  if (server.arg("x_data") == "") {   //Parameter not found
    message = "liner_lost";
  } else {    //Parameter found
    x_data = server.arg("x_data");

    y_data = server.arg("y_data");
    String data_ = server.arg("size_data");
    size_data = data_.toInt();
    //Serial.print("x_data");
    // Serial.println(x_data);
    // Serial.print("y_data");
    //Serial.println(y_data);
    //Serial.print("size_data");
    //Serial.println(size_data);
    message = stringTodata(x_data, y_data, size_data);

  }

  server.send(200, "text/plain", message);

}
void signal_data()
{

  String message = "";
  //sample_data();
 if (page_active != 1)
  {
    p_abs.show();
  }
  page_active = 1;
  message = String(s_red, 3) + "," + String(s_green, 3) + "," + String(s_blue, 3);
  server.send(200, "text/plain", message);


}
void version_data() {
  server.send(200, "text/plain", "minispec2019v2");
}
void data_read() {

  String message = "";
  //sample_data();
   if (page_active != 1)
  {
    p_abs.show();
  }
  page_active = 1;
  
  if (s_red != 0)
  {
    abs_red = log(b_red / s_red);
  } else {
    abs_red = 0;
  }
  if ( s_green != 0)
  {
    abs_green = log(b_green / s_green);
  } else
  {
    abs_green = 0;
  }

  if (s_blue != 0)
  {
    abs_blue = log(b_blue / s_blue);
  } else
  {
    abs_blue = 0;
  }




  message = String(abs_red, 3) + "," + String(abs_green, 3) + "," + String(abs_blue, 3);
  server.send(200, "text/plain", message);

}

void websample_read() {

  String message = "";
  //sample_data();
  if (s_red != 0)
  {
    abs_red = log(b_red / s_red);
  } else {
    abs_red = 0;
  }
  if ( s_green != 0)
  {
    abs_green = log(b_green / s_green);
  } else
  {
    abs_green = 0;
  }

  if (s_blue != 0)
  {
    abs_blue = log(b_blue / s_blue);
  } else
  {
    abs_blue = 0;
  }




  message = String(abs_red, 3) + "," + String(abs_green, 3) + "," + String(abs_blue, 3);
  server.send(200, "text/plain", message);

}
void set_blank() {

  String message = "set_blank";
 // sample_data();
  b_red = s_red;
  b_green = s_green;
  b_blue = s_blue;
  server.send(200, "text/plain", message);
  

}



void sample_data()
{

 
  // Serial.print("led_typ = ");Serial.println(led_typ);
if(led == 0)
{
  
  if (led_typ == 1)
  {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, HIGH);
    digitalWrite(blue_led, HIGH);
  } else
  {
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, LOW);
  }
  
delay(1);
  s_red =  sensor_read();
  led ++;
}else if(led == 1)
{
  
    if (led_typ == 1 )
  {
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, HIGH);
  } else 
  {
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, HIGH);
    digitalWrite(blue_led, LOW);
  }

  delay(1);

  s_green = sensor_read();
  led ++;
}else if(led == 2)
{
  if (led_typ == 1  )
  {
   digitalWrite(red_led, HIGH);
   digitalWrite(green_led, HIGH);
   digitalWrite(blue_led, LOW);
  } else
  {
   digitalWrite(red_led, LOW);
   digitalWrite(green_led, LOW);
   digitalWrite(blue_led, HIGH);
  }

 delay(1);
  s_blue =  sensor_read();
  led ++;
}
  

  if (led_typ == 1)
  {
    digitalWrite(red_led, HIGH);
   digitalWrite(green_led, HIGH);
    digitalWrite(blue_led, HIGH);
  } else
  {
   digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
    digitalWrite(blue_led, LOW);
  }




}

String linearOut(float inputX[], float inputY[], int Size)
{

  double x_mean = 0;
  double y_mean = 0;
  double x_step1 = 0;
  double y_step1 = 0;
  double x_step2 = 0;
  double y_step2 = 0;
  double arrayN = 0;
  double sumYstep = 0;
  double sumYstep2 = 0;
  double coutSum = 0;
  double op = 2, rSqua = 0;


  for (int i = 0; i <= Size - 1; i++) {

    coutSum = coutSum + inputX[i];

  }
  x_mean = coutSum / Size;


  coutSum = 0;
  for (int i = 0; i <= Size - 1; i++) {

    coutSum = coutSum + inputY[i];

  }
  y_mean = coutSum / Size;

  for (int i = 0; i <= Size - 1; i++) {

    x_step1 = ((inputX[i] - x_mean) * (inputY[i] - y_mean));

    x_step2 = x_step2 + x_step1;
    y_step1 = pow((inputX[i] - x_mean), 2);
    y_step2 = y_step2 + y_step1;
    sumYstep = sumYstep + (pow ((inputY[i] - y_mean), 2));
  }

  Slope = x_step2 / y_step2;



  Inter = y_mean - (Slope * x_mean);

  for (int i = 0; i <= Size - 1; i++)
  {
    sumYstep2 = sumYstep2 + (pow ((((inputX[i] * Slope) + Inter) - y_mean), 2));
  }

  rSqua = sumYstep2 / sumYstep;
  String data_return = String(Slope, 4) + "," + String(Inter, 4) + "," + String(rSqua, 4);
  return data_return;

}

void configureSensor()
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
 // tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
    tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  // tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)

  if (setting_resolution == 1)
  { tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);

  } else if (setting_resolution == 2)
  { tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);

  } else if (setting_resolution == 3)
  {
    tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);
  } else
  { tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);

  }

}




float sensor_read()
{


  uint16_t x = 0;

 
 x =  tsl.getLuminosity(TSL2561_VISIBLE);


 
  /* Populate broadband and infrared with the latest values */
//  uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
//  x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  
//uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);     
  //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);
  // tsl.getLuminosity (&broadband, &infrared);
  // Serial.print("Sensor_data = "); Serial.println(x);

  return x;

}
void bee(int delayTime)
{


}

void blank()
{
 // b_red = 0, b_green = 0, b_blue = 0;

 // sample_data();
  b_red = s_red;
  b_green = s_green;
  b_blue = s_blue;

}

void bc_set()
{
  cal_red.setValue(cal_colo[0]);
  cal_green.setValue(cal_colo[1]);
  cal_blue.setValue(cal_colo[2]);
  if (cal_colo[0] == 1)
  {
    cal_a1.Set_background_color_bco(63488);
    cal_a2.Set_background_color_bco(63488);
    cal_a3.Set_background_color_bco(63488);
    cal_a4.Set_background_color_bco(63488);
    cal_a5.Set_background_color_bco(63488);
  }
  if (cal_colo[1] == 1)
  {
    cal_a1.Set_background_color_bco(864);
    cal_a2.Set_background_color_bco(864);
    cal_a3.Set_background_color_bco(864);
    cal_a4.Set_background_color_bco(864);
    cal_a5.Set_background_color_bco(864);
  }
  if (cal_colo[2] == 1)
  {
    cal_a1.Set_background_color_bco(31);
    cal_a2.Set_background_color_bco(31);
    cal_a3.Set_background_color_bco(31);
    cal_a4.Set_background_color_bco(31);
    cal_a5.Set_background_color_bco(31);

  }
  cal_a1.Set_font_color_pco(65535);
  cal_a2.Set_font_color_pco(65535);
  cal_a3.Set_font_color_pco(65535);
  cal_a4.Set_font_color_pco(65535);
  cal_a5.Set_font_color_pco(65535);

}

void R_update(int val)
{
  if (cal_colo[0] == 0)
  {
    val = 0;
  }
  if (s_red == 0)
  {
    abs_ = 0;
  } else
  {
    abs_ = log(b_red / s_red);
  }

  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_, 4, 2, buffer);

  if (page_active == 1)
  {
    abs_r.setText(buffer);
  }
  else if (page_active == 2 && val == 1)
  {
    cal_a1.setText(buffer);
    abs_data[0] = abs_;
  }
  else if (page_active == 2 && val == 2)
  {
    cal_a2.setText(buffer);
    abs_data[1] = abs_;
  }
  else if (page_active == 2 && val == 3)
  {
    cal_a3.setText(buffer);
    abs_data[2] = abs_;
  }
  else if (page_active == 2 && val == 4)
  {
    cal_a4.setText(buffer);
    abs_data[3] = abs_;
  }
  else if (page_active == 2 && val == 5)
  {
    cal_a5.setText(buffer);
    abs_data[4] = abs_;
  }
  else if (page_active == 3 && cal_colo[0] == 1)
  {
    con_abs.setText(buffer);
    abs_data[0] = abs_;
    con_data[0] = (abs_ - Inter) / Slope;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(con_data[0], 4, 2, buffer);
    con_con.setText(buffer);
  }


}

void G_update(int val)
{
  if (cal_colo[1] == 0)
  {
    val = 0;
  }
  memset(buffer, 0, sizeof(buffer));
  if (s_green == 0)
  {
    abs_ = 0;
  } else
  {
    abs_ = log(b_green / s_green);
  }


  dtostrf(abs_, 4, 2, buffer);
  if (page_active == 1)
  {
    abs_g.setText(buffer);
  } else if (page_active == 2 && val == 1)
  {
    cal_a1.setText(buffer);
    abs_data[0] = abs_;
  }
  else if (page_active == 2 && val == 2)
  {
    cal_a2.setText(buffer);
    abs_data[1] = abs_;
  }
  else if (page_active == 2 && val == 3)
  {
    cal_a3.setText(buffer);
    abs_data[2] = abs_;
  }
  else if (page_active == 2 && val == 4)
  {
    cal_a4.setText(buffer);
    abs_data[3] = abs_;
  }
  else if (page_active == 2 && val == 5)
  {
    cal_a5.setText(buffer);
    abs_data[4] = abs_;
  }
  else if (page_active == 3 && cal_colo[1] == 1)
  {
    con_abs.setText(buffer);
    abs_data[0] = abs_;
    con_data[0] = (abs_ - Inter) / Slope;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(con_data[0], 4, 2, buffer);
    con_con.setText(buffer);
  }
  /*else if(page_active == 3)
    {
    abs_con.setText(buffer);
    con_ = (abs_-Inter)/Slope;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(con_,4,2,buffer);
    con_con.setText(buffer);
    }else if(page_active == 6)
    {
    // s_green = random(0,1000);
    memset(buffer, 0, sizeof(buffer));
    itoa(s_green, buffer, 10);
    Green_abs.setText(buffer);
    }else if(page_active == 2)
    {
    Green_abs.setText(buffer);
    }*/

}
void B_update(int val)
{
  if (cal_colo[2] == 0)
  {
    val = 0;
  }
  //s_blue = random(0,10);
  memset(buffer, 0, sizeof(buffer));
  if (s_blue == 0)
  {
    abs_ = 0;
  } else
  {
    abs_ = log(b_blue / s_blue);
  }

  dtostrf(abs_, 4, 2, buffer);
  if (page_active == 1)
  {
    abs_b.setText(buffer);
  } else if (page_active == 2 && val == 1)
  {
    cal_a1.setText(buffer);
    abs_data[0] = abs_;
  }
  else if (page_active == 2 && val == 2)
  {
    cal_a2.setText(buffer);
    abs_data[1] = abs_;
  }
  else if (page_active == 2 && val == 3)
  {
    cal_a3.setText(buffer);
    abs_data[2] = abs_;
  }
  else if (page_active == 2 && val == 4)
  {
    cal_a4.setText(buffer);
    abs_data[3] = abs_;
  }
  else if (page_active == 2 && val == 5)
  {
    cal_a5.setText(buffer);
    abs_data[4] = abs_;
  }
  else if (page_active == 3 && cal_colo[2] == 1)
  {
    con_abs.setText(buffer);
    abs_data[0] = abs_;
    con_data[0] = (abs_ - Inter) / Slope;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(con_data[0], 4, 2, buffer);
    con_con.setText(buffer);
  }
  /*else if(page_active == 3)
    {
    abs_con.setText(buffer);
    con_ = (abs_-Inter)/Slope;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(con_,4,2,buffer);
    con_con.setText(buffer);
    }else if(page_active == 6)
    {
    // s_blue = random(0,1000);
    memset(buffer, 0, sizeof(buffer));
    itoa(s_blue, buffer, 10);
    Blue_abs.setText(buffer);
    }else if(page_active == 2)
    {
    Blue_abs.setText(buffer);
    }
  */
}




void gotable1(int data)
{
  tab_la1.setText("M");
  tab_la2.setText("C");
  std_name(data);
  tab_n1.setText(buffer);
  std_name(data + 1);
  tab_n2.setText(buffer);
  std_name(data + 2);
  tab_n3.setText(buffer);
  std_name(data + 3);
  tab_n4.setText(buffer);
  std_name(data + 4);
  tab_n5.setText(buffer);
  std_name(data + 5);
  tab_n6.setText(buffer);
  std_name(data + 6);
  tab_n7.setText(buffer);

  memset(buffer, 0, sizeof(buffer));
  float data_ = std_m(data);
  dtostrf(data_, 5, 3, buffer);
  tab_ab1.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 1);
  dtostrf(data_, 5, 3, buffer);
  tab_ab2.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 2);
  dtostrf(data_, 5, 3, buffer);
  tab_ab3.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 3);
  dtostrf(data_, 5, 3, buffer);
  tab_ab4.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 4);
  dtostrf(data_, 5, 3, buffer);
  tab_ab5.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 5);
  dtostrf(data_, 5, 3, buffer);
  tab_ab6.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_m(data + 6);
  dtostrf(data_, 5, 3, buffer);
  tab_ab7.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data);
  dtostrf(data_, 5, 3, buffer);
  tab_co1.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 1);
  dtostrf(data_, 5, 3, buffer);
  tab_co2.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 2);
  dtostrf(data_, 5, 3, buffer);
  tab_co3.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 3);
  dtostrf(data_, 5, 3, buffer);
  tab_co4.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 4);
  dtostrf(data_, 5, 3, buffer);
  tab_co5.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 5);
  dtostrf(data_, 5, 3, buffer);
  tab_co6.setText(buffer);
  delay(10);
  memset(buffer, 0, sizeof(buffer));
  data_ = std_c(data + 6);
  dtostrf(data_, 5, 3, buffer);
  tab_co7.setText(buffer);


}


void updatecal_c()
{
  memset(buffer, 0, sizeof(buffer));
  dtostrf(con_data[0], 5, 2, buffer);
  cal_c1.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(con_data[1], 5, 2, buffer);
  cal_c2.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(con_data[2], 5, 2, buffer);
  cal_c3.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(con_data[3], 5, 2, buffer);
  cal_c4.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(con_data[4], 5, 2, buffer);
  cal_c5.setText(buffer);

  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_data[0], 5, 2, buffer);
  cal_a1.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_data[1], 5, 2, buffer);
  cal_a2.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_data[2], 5, 2, buffer);
  cal_a3.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_data[3], 5, 2, buffer);
  cal_a4.setText(buffer);
  memset(buffer, 0, sizeof(buffer));
  dtostrf(abs_data[4], 5, 2, buffer);
  cal_a5.setText(buffer);

}
void doubleTorom (int adds, float valRom)
{
  char finalArray[4];
  char* byteArray = reinterpret_cast<char*>(&valRom);
  int i = 0;
  for (adds + i; adds + i < adds + 4; i++)
  {
    EEPROM.write(adds + i, byteArray[i]);
  }
  EEPROM.commit();
}

float romTodouble (int adds)
{
  char finalArray[4];
  float final;
  int i = 0;
  for (adds + i; adds + i < adds + 4; i++)
  {
    finalArray[i] = EEPROM.read(adds + i);
  }
  final = *reinterpret_cast<float*>(finalArray);
  return final;


}
float std_m(int index_)
{
  float data = romTodouble(140 + (index_ * 4));

  return data;

}
float std_c(int index_)
{
  float data = romTodouble(200 + (index_ * 4));

  return data;
}

int read_stdled(int index_)
{

  int led = EEPROM.read(260 + index_);
  for (int i = 0; i < 3; i++)
  {
    if (led == i)
    {
      cal_colo[i] = 1;
    } else
    {
      cal_colo[i] = 0;
    }

  }

  return led;

}

void write_std(int index_, String name_, float m_, float c_)
{
  memset(buffer, 0, sizeof(buffer));
  name_.toCharArray(buffer, sizeof(buffer));
  for (int i = 0; i < 10; i++)
  {
    EEPROM.write((index_ * 10) + i, buffer[i]);
  }

  doubleTorom(140 + (index_ * 4), m_);
  doubleTorom(200 + (index_ * 4), c_);

  for (int i = 0; i < 3; i++)
  {
    if (cal_colo[i] == 1)
    {
      EEPROM.write(260 + index_, i);
    }

  }

  EEPROM.commit();



}

void sd_save(String name_, String dataString)
{
  if (SD.exists(name_)) {
    myFile = SD.open(name_ , FILE_WRITE);
    if (myFile)
    {
      myFile.print(dataString);
      myFile.print(","); delay(1);
      myFile.print(abs_data[0], 3);
      myFile.print(","); delay(1);
      myFile.println(con_data[0], 3);
      myFile.flush();
      myFile.close();
    }

  } else
  {
    myFile = SD.open(name_ , FILE_WRITE);
    if (myFile)
    {
      myFile.println("RGB Minispectro");

      myFile.println("Name,Absortion,Concentration");
      myFile.print(dataString);
      myFile.print(","); delay(1);
      myFile.print(abs_data[0], 3);
      myFile.print(","); delay(1);
      myFile.println(con_data[0], 3);
      myFile.flush();
      myFile.close();
    }

  }


}

void kin_save(String name_, long time_, float R_abs, float G_abs, float B_abs)
{
  memset(buffer, 0, sizeof(buffer));
  name_.toCharArray(buffer, sizeof(buffer));
  kin_lable.setText(buffer);
  if (SD.exists(name_)) {
    myFile = SD.open(name_ , FILE_WRITE);
    if (myFile)
    {
      myFile.print(time_);
      myFile.print(",");
      myFile.print(R_abs, 3);
      myFile.print(",");
      myFile.print(G_abs, 3);
      myFile.print(",");
      myFile.println(B_abs, 3);
      myFile.flush();
      myFile.close();
    }

  } else
  {
    myFile = SD.open(name_ , FILE_WRITE);
    if (myFile)
    {
      myFile.println("RGB Minispectro");
      myFile.println("Time,Red,Green,Blue");
      delay(1);
      myFile.flush();
      myFile.close();
    }

  }



}

void kin_add()
{
  if (kin_ == 1)
  {

    abs_data[0] = log(b_red / s_red);
    abs_data[1] = log(b_green / s_green);
    abs_data[2] = log(b_blue / s_blue);
    //dbSerialPrintln(abs_data[2]);
    s0.addValue(0, abs_data[0] * 80);
    s0.addValue(1, abs_data[1] * 80);
    s0.addValue(2, abs_data[2] * 80);
    memset(buffer, 0, sizeof(buffer));
    dtostrf(abs_data[0], 5, 2, buffer);
    kin_r.setText(buffer);
    memset(buffer, 0, sizeof(buffer));
    dtostrf(abs_data[1], 5, 2, buffer);
    kin_g.setText(buffer);
    memset(buffer, 0, sizeof(buffer));
    dtostrf(abs_data[2], 5, 2, buffer);
    kin_b.setText(buffer);
    kin_timeRevo = kin_timeRevo + tim_kin;
    memset(buffer, 0, sizeof(buffer));
    dtostrf(kin_timeRevo, 5, 0, buffer);
    kin_lable.setText(buffer);
    kin_save(sd_name, kin_timeRevo, abs_data[0], abs_data[1], abs_data[2]); /// name   time  r g b
  }

}

void std_name(int index_)
{
  memset(buffer, 0, sizeof(buffer));
  String data_ck = "";
  for (int i = 0; i < 10; i++)
  {
    buffer[i] = EEPROM.read((index_ * 10) + i);

  }



}

void handleRoot() {
  server.sendHeader("Location", "/index.html", true);  //Redirect to our html web page
  server.send(302, "text/plane", "");

  p_abs.show();

  page_active = 1;
}

bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";
  fs::File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}
void handleWebRequests() {
  if (loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}



void SD_file_download(String filename){
  
    File download = SD.open(filename);
    if (download) {
      server.sendHeader("Content-Type", "text/html");
      server.sendHeader("Content-Disposition", "attachment; filename="+filename);
      server.sendHeader("Connection", "close");
      server.streamFile(download, "application/octet-stream");
      download.close();
    } 
  
}
