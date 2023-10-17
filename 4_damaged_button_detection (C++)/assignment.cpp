#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int total=0, xmin=0, xmax=0, ymin=0, ymax=0; 
int screenx, screeny, maxcolours; 
int coordinates[200][200];

class pixel_class {
private:
  int red, green, blue;
  bool exclude;  // if true, do not check this pixel
public:
  void loaddata(int v1, int v2, int v3);
  void datatofile(fstream & ppmfile);
  int getR() { return red; }
  int getG() { return green; }
  int getB() { return blue; }
  void setexclude(bool ex) { exclude = ex; }
  bool getexclude() { return exclude; }
  void display(){cout<<" R:"<<getR()<<" G:"<<getG()<<"B:"<<getB()<<"   excluded:"<<getexclude()<<endl;}
};
void loadButtons();
void identifyButtons();
void findPixels(int,int);
void findConnectedPixels(int,int);
void drawBox(int,int,int);
void pixels_to_image(string);

//picture variable declared after the class because it's type is pixel_class which is the class itself
pixel_class picture[600][600];

     
int main() {
  // Step 1 : read in the image from Buttons.ppm
  loadButtons();


  // Step 2 : identify buttons and draw boxes
  //cout<<"starting the process of detecting damaged buttons";

  //looping over the screen (axes x and y) to get every pixel in the screen
  //and checking if the value of Red in pixel is greater than 128 and its not checked already
  //if its the cas we initialise the values of xmin,xmax,ymin,ymax and total to be able to draw the boxes later
  //finaly we call the function findConnectedPixels over each pixel to get all the pixels that are connected to
  //the current one 
  for (int y=0; y<screeny; y++){
    for (int x=0; x<screenx; x++){
      if(picture[x][y].getR()>128 && picture[x][y].getexclude()==false){
        xmin=x;
        xmax=x;
        ymin=y;
        ymax=y;
        total=0;

        findConnectedPixels(x,y);

        //drawing the Box
        //any button that has more than 7900 pixels where Red value is greater thean 128 is 
        //considered as undamanged, and if it has less than that which means that there are 
        //a lot o missings white area in the button we consider it as damaged and the box around it is red
        if(total>5300) 
          // (0,255,0) is the RGB value of Green color
          drawBox(0,255,0);
        else 
          //(255,0,0) is the RGB value of Red color
          drawBox(255,0,0);
      }
    }
  }

  
  // Step 3 : output the final .ppm file
  //puting the values stored in screen[600][600] in ppm file
  //using the same process in loadButtons()
  pixels_to_image("damaged_buttons.ppm");


  //cout << "Done detecting damaged buttons, check for 'damaged_buttons.ppm' and convert to bmp to see the results";
}

void loadButtons() {
  // load the picture from Buttons.ppm
  //cout<<"loading the picture Buttons.ppm"<<std::endl;
  int x, y, R, G, B;
  fstream infile;
  string infilename, line;
  infilename = "testcase.ppm";
  infile.open(infilename.c_str(), fstream::in);
  if (infile.is_open() == false) {
    cout << "ERROR: not able to open " << infilename << endl;
    exit(2);
  }
  getline(infile, line);  // this line is "P3"
  getline(infile, line);  // this line is "# filename"
  infile >> screenx >> screeny;  // this line is the size
  infile >> maxcolours;  // this line is 256
  for (y = 0; y < screeny; y++) {
    for (x = 0; x < screenx; x++) {
      infile >> R >> G >> B;
      picture[x][y].loaddata(R, G, B);

      picture[x][y].setexclude(false);
    }
  }
  infile.close();
}

void findConnectedPixels(int x,int y){
  //cout<<"Finding connected pixels to ("<<x<<","<<y<<")"<<endl;
  int red=picture[x][y].getR();
  bool excluded;

  //skipping the current pixel if it was already checked or if its color is not "white"  
  if (red <=128 || picture[x][y].getexclude()==true){
    picture[x][y].setexclude(true);
    return;
  }
  //if the pixel was not excluded already we update the values of xmin,xmax,ymin,ymax
  //and we increase total to indicate the number of pixels that belong in a single button
  total++;
  if (x<xmin) xmin=x;
  if (x>xmax) xmax=x;
  if (y<ymin) ymin=y;
  if (y>ymax) ymax=y;

  //cout<<"A pixel of a button was detected.."<<endl;
  //we exclude the current pixel as it was detected already
  picture[x][y].setexclude(true);

  //we call our function on the neighbors directly connected to the current pixel (top,bottom,left,right)
  if(x>0){findConnectedPixels(x-1,y);}
  if(y>0){findConnectedPixels(x,y-1);}
  if(x<screenx-1){findConnectedPixels(x+1,y);}
  if(y<screeny-1){findConnectedPixels(x,y+1);}

}

void drawBox(int r, int g, int b){
  //cout<<"Drawing the Boxes"<<endl;
  //function to draw the Bow
  //the function takes the values r,g,b as parameters so we will be able to draw
  //both red and green boxes depending on the case of the button
  int x,y;

  //drawing the vertical lines of the box
  for(x=xmin; x<=xmax; x++){
    picture[x][ymin].loaddata(r,g,b);
    picture[x][ymin].setexclude(true);
    picture[x][ymax].loaddata(r,g,b);
    picture[x][ymax].setexclude(true);
  }

  //drawing the horizontal lines of the box
  for(y=ymin; y<=ymax; y++){
    picture[xmin][y].loaddata(r,g,b);
    picture[xmin][y].setexclude(true);
    picture[xmax][y].loaddata(r,g,b);
    picture[xmax][y].setexclude(true);
  }
}

void pixels_to_image(string outfilename){
  //cout<<"Saving the results in ppm file"<<endl;

  // Saving the results in ppm file using the same concept in loadButtons
  fstream outfile;
  outfile.open(outfilename.c_str(), fstream::out);
  if (outfile.is_open() == false) {
    cout << "ERROR: not able to open " << outfilename << endl;
    exit(2);
  }
  outfile << "P3" << endl;
  outfile << "# Modified " << outfilename << endl;
  outfile << screenx << " " << screeny << endl;
  outfile << maxcolours << endl;

  for (int y = 0; y < screeny; y++) {
    for (int x = 0; x < screenx; x++) {
      outfile << picture[x][y].getR() << " " << picture[x][y].getG() << " " << picture[x][y].getB() << " ";
    }
    outfile << endl;
  }
  outfile.close();


  cout << "Results saved as " << outfilename << endl;

}
//--------------- methods for the pixel_class ------------
void pixel_class::loaddata(int v1, int v2, int v3) {
  red = v1;
  green = v2;
  blue = v3;
}

void pixel_class::datatofile(fstream & ppmfile) {
  // write the data for one pixel to the ppm file
  ppmfile << red << " " << green;
  ppmfile << " " << blue << "  ";
}




void findConnectedPixels2(int x,int y){
  cout<<"insde findConnectedPixels"<<endl;
  if ((picture[x][y].getR()<=128) || (picture[x][y].getexclude()==true)) {
    //total=0;xmin=screenx;xmax=0;ymin=screeny;ymax=0;
    cout<<"returning"<<endl;
    return;
    
  }
  else{
    picture[x][y].setexclude(true);
    cout<<"*****pixel of a button found in ("<<x<<","<<y<<")"<<endl;
    if (x<xmin) xmin=x;
    if (x>xmax) xmax=x;
    if (y<ymin) ymin=y;
    if (y>ymax) ymax=y;
    total++;
    cout<<"UPDATE X="<<x<<" Y="<<y<<" xmin="<<xmin<<" xmax="<<xmax<<" ymin="<<ymin<<" ymax="<<ymax<<" total="<<total<<endl;
    //if xmax and ymax stopped increasing which means that we no longer find connected pixels 
    //to the buttons we store the coordonates xmin xmax ymin ymax in an array to draw them later
    //total=0;xmin=screenx;xmax=0;ymin=screeny;ymax=0;
      
    if (xmax==x && ymax==y){
        cout<<"ADDING to the array";
        for (int i=0;i<200;i++){
          for (int j=0;j<200;j++){
            if (coordinates[i][j]==0){
              cout<<"added these value to ("<<x<<","<<y<<") of the array = xmin"<<xmin<<" xmax"<<xmax<<" ymin"<<ymin<<" ymax"<<endl;
              coordinates[i][j]=xmin;
              coordinates[i][j+1]=xmax;
              coordinates[i][j+2]=ymin;
              coordinates[i][j+3]=ymax;
              coordinates[i][j+4]=total;
              return;
            }
          }
      }
    }
    findConnectedPixels(x-1,y);
    findConnectedPixels(x+1,y);
    findConnectedPixels(x,y-1);
    findConnectedPixels(x,y+1);
    
  }
 
}
void findPixels(int x,int y){
  int finding_pixels = 0;
  while((picture[x][y].getR()>128) && !(picture[x][y].getexclude()))
  {
    finding_pixels++;
    cout<<"pixel of a button found in ("<<x<<","<<y<<")"<<endl;
    picture[x][y].display();
    //excluding the found pixel and finding its neighbhors
    picture[x][y].setexclude(true);

    total=total+1;
    if (x<xmin) xmin=x;
    else if (x>=xmax) xmax=x;
    if (y<ymin) ymin=y;
    else if (y>=ymax) ymax=y;

    cout<<"Total = "<<total<<"xmin : "<<xmin<<"xmax :"<<xmax<<"ymin :"<<ymin<<" ymax"<<ymax<<endl;

    //find the neighbhors of the found pixel
    findPixels(x-1,y);
    findPixels(x+1,y);
    findPixels(x,y-1);
    findPixels(x,y+1);
  }

}

void drawBox2(int xmin,int xmax,int ymin,int ymax,int redvalue,int greenvalue){
  cout<<"drawing the box"<<endl;
  //the RGB value f the color green is (0,255,0) and red is (255,0,0)
  //we used redvalue,greenvalue in params to be able to choose the color of the box

  if(xmin!=0 && xmin!=0 && ymin!=0 & ymax!=0){
    //drawing the horizontal lines of the box
    for (int x = xmin; x <= xmax; x++) { 
      picture[x][ymin].loaddata(redvalue, greenvalue, 0); 
      picture[x][ymax].loaddata(redvalue, greenvalue, 0); 
      picture[x][ymin].setexclude(true);
      picture[x][ymax].setexclude(true);
    }

    //drawing the vzrtical lines of the box
    for(int y = ymin; y <=ymax; y++){
      picture[xmin][y].loaddata(redvalue, greenvalue, 0); 
      picture[xmax][y].loaddata(redvalue, greenvalue, 0); 
      picture[xmin][y].setexclude(true);
      picture[xmax][y].setexclude(true);
    }

    // Save the modified image
    fstream outfile;
    string infilename = "Buttons.ppm";
    string outfilename = "Buttons2_modified.ppm";
    outfile.open(outfilename.c_str(), fstream::out);
    
    outfile << "P3" << endl;
    outfile << "# Modified " << infilename << endl;
    outfile << screenx << " " << screeny << endl;
    outfile << maxcolours << endl;

    for (int y = 0; y < screeny; y++) {
        for (int x = 0; x < screenx; x++) {
            outfile << picture[x][y].getR() << " " << picture[x][y].getG() << " " << picture[x][y].getB() << " ";
        }
        outfile << endl;
    }

    outfile.close();
    
    cout << "Modified image saved as " << outfilename << endl;

  }
}
