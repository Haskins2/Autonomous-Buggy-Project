import controlP5.*;
import processing.net.*;
Client myClient;
String data;
int idata;
PFont f;
ControlP5 p5;
Knob myKnobA;

Textarea myTextarea1;
Textarea myTextarea2;
Textarea myTextarea3;
Textlabel myButtonlabelA;
Textlabel myButtonlabelB;

void setup(){
  frameRate(144);
  myClient = new Client(this,"192.168.4.1",5200);
  //myClient.write("I am a new Client");
  size(800,1000);
  
  p5 = new ControlP5(this);
  Button GOButton = p5.addButton("W")
  .setPosition(100, 100)
  .setSize(600,100)
  .setColorLabel(color(0))
  .setColorBackground(color(0))
  .setColorActive(color(255));
  
  p5 = new ControlP5(this);
  Button STOPButton = p5.addButton("S")
  .setPosition(100, 300)
  .setSize(600,100)
  .setColorLabel(color(0))
  .setColorBackground(color(0))
  .setColorActive(color(255));
  
  myButtonlabelA = p5.addTextlabel("label")
  .setText("GO")
  .setPosition(360,130)
  .setColorValue(0xff00ff00)
  .setFont(createFont("Georgia",40));
  
  myButtonlabelB = p5.addTextlabel("label1")
  .setText("STOP")
  .setPosition(345,330)
  .setColorValue(0xffff0000)
  .setFont(createFont("Georgia",40));
  
  myKnobA = p5.addKnob("o")
  .setRange(0,25)
  .setValue(0)
  .setPosition(200,500)
  .setRadius(200)
  .setDragDirection(Knob.VERTICAL);
  
  myTextarea1 = p5.addTextarea("txt")
  .setPosition(100,400)
  .setSize(200,50)
  .setFont(createFont("arial",20))
  .setLineHeight(20)
  .setColor(color(255))
  .setColorBackground(color(255,100))
  .setColorForeground(color(255,100));
  
  myTextarea2 = p5.addTextarea("txt2")
  .setPosition(300,400)
  .setSize(200,50)
  .setFont(createFont("arial",20))
  .setLineHeight(20)
  .setColor(color(255))
  .setColorBackground(color(255,100))
  .setColorForeground(color(255,100));
  
  myTextarea3 = p5.addTextarea("txt3")
  .setPosition(500,400)
  .setSize(200,50)
  .setFont(createFont("arial",20))
  .setLineHeight(20)
  .setColor(color(255))
  .setColorBackground(color(255,100))
  .setColorForeground(color(255,100));
  
  f = createFont("Arial",16);
  textFont(f);
}

void draw() {
  background(color(150));

  //Silver
  idata = myClient.read();
  if(idata < 125 && idata > 0){
  println("Object Detected at : " + idata + "cm");
  myTextarea1.setText("Object Detected at : " + idata + "cm");
  }
  
  idata = myClient.read();
  
  if(idata != -1 && idata < 25){
  println("Encoder Speed: " + idata);
  myTextarea2.setText("Encoder Speed: " + idata + "cm/s");
  myKnobA.setValue(float(idata));
  }
  
  idata = myClient.read();
  if(idata != -1 && idata < 25){
  println("Ultrasonic Speed: " + idata);
  myTextarea3.setText("UltraSonic Speed: " + idata + "cm/s");
  }
}

public void controlEvent(ControlEvent ev){
  myClient.write(ev.getController().getName());
}
