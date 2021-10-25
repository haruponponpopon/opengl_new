#include <cstring>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <climits>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "display.h"
using namespace std;
#define SUNSET_START (20)

void draw_task(string text);
void draw_rect2D_fill(double x, double y);
void draw_rect2D_line(double x, double y);
void draw_string(string &text, xy position);
void Circle2DFill(float radius,int x,int y);
void draw_around_sun(double s_rad);
void draw_line(xy a, xy b);
void draw_triangle(xy center, double r, double s_rad);
void Circle2D(float radius,int x,int y);

// ディスプレイに変化があった時に呼び出されるコールバック関数。
// 「ディスプレイに変化があった時」は、glutPostRedisplay() で指示する。
void glut_display(){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(50.0, 1.0, 0.1, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,3,10,0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                  // 今まで画面に描かれていたものを消す
    if (g_display_mode == 1){
        glClearColor(0.4f, 0.9f, 0.9f, 0.2f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //display ground
        glColor4f(0.0, 0.9, 0.0,0.0);
        Circle2DFill(10,0,-12);
        //display player name
        glColor4f(0.0, 0.0, 0.7, 0.0);
        string str = "enter player name";
        draw_string(str, {70, 120});
        draw_string(player_name, {105,120});
        draw_line({65,115},{130,115});
        glColor4f(1.0, 0.0, 0.0, 0.0);
        if (ordinary_timer/10%4){
            string moji = "press enter to start";
            draw_string(moji, {90, 50});
        }
        glColor4f(1.0, 0.0, 0.0,0.0);
        Circle2DFill(20,200,200);
        //draw cloud
        glColor3d(1.0,1.0,1.0); //white
        Circle2DFill(12,95,165);
        Circle2DFill(12,105,165);
        Circle2DFill(12,88,160);
        Circle2DFill(12,100,155);
        Circle2DFill(12,112,160);
        glColor4f(1.0, 0.5, 0.0, 0.0);
        str = "SUNSET TYPING";
        draw_string(str, {90, 160});
        glutSwapBuffers();
        glFlush();
    }else if (g_display_mode == 2){   
        GLfloat lightpos[] = {0,30,0};
        GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
        GLfloat lightpos1[] = {0,0,30};
        GLfloat diffuse1[] = {1.0, 1.0, 1.0, 1.0};
        //sunset
        if (time_limit<SUNSET_START*TIME_REDUCTION_SPEED){
            float par = time_limit*1.0/(SUNSET_START*TIME_REDUCTION_SPEED);//getting smaller
            diffuse[1] = 0.6+0.4*par;
            diffuse[2] = 0.4+0.6*par;
            lightpos[1] = 30*sin(par*M_PI/2);
            lightpos[2] = 30*cos(par*M_PI/2);
            if (time_limit>2*TIME_REDUCTION_SPEED){
                par = (time_limit-2*TIME_REDUCTION_SPEED)*1.0/((SUNSET_START-2)*TIME_REDUCTION_SPEED);
                glClearColor(1.0f-0.6*par*par, 0.9f-0.4*(1-par)*(1-par), 0.9f-0.6*(1-par)*(1-par), 0.2f);
                diffuse1[0] = diffuse1[1]=diffuse1[2]=0.5+0.5*par;
            }else{
                par = time_limit*1.0/(2*TIME_REDUCTION_SPEED);
                glClearColor((1-cos(par*M_PI))/2*(1-0.05)+0.05, (1-cos(par*M_PI))/2*(0.5-0.05)+0.05,0.3, 0.2f);
                diffuse1[0] = diffuse1[1]=diffuse1[2]=0.5*par;
            }
        } 
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        

        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
        glLightfv(GL_LIGHT1, GL_POSITION, lightpos1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
        //display tasks
        int count = 0;
        for (auto task: task_image_data){
            if (task.status==0){
                glPushMatrix();
                // glColor3f(1.0, 1.0, 1.0);
                glTranslatef(task.position.x, task.position.y+max(sin((time_limit+count*4)%32*M_PI/16)-0.5,0.0), task.position.z);
                draw_task(task.title);
                glPopMatrix();
            }else if (task.status<20){
                double move_x = 0.1*task.status;
                double move_y = -0.01*task.status*(task.status-20);
                glPushMatrix();
                // glColor3d(1.0, 0.0, 0.0);
                GLfloat facecolor[] = {1.0,0.0,0.0,1.0};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, facecolor);
                glTranslated(task.position.x+move_x, task.position.y+move_y, task.position.z);
                glutSolidSphere(0.1, 5,5);
                glPopMatrix();
                glPushMatrix();
                // glColor3d(0.0, 1.0, 0.0);
                GLfloat facecolor1[] = {0.0,1.0,0.0,1.0};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, facecolor1);
                glTranslated(task.position.x-move_x, task.position.y+move_y, task.position.z);
                glutSolidSphere(0.1, 5,5);
                glPopMatrix();
                move_y *= 2;
                glPushMatrix();
                // glColor3d(1.0, 1.0, 0.0);
                GLfloat facecolor3[] = {1.0,1.0,0.0,1.0};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, facecolor3);
                glTranslated(task.position.x+move_x, task.position.y+move_y, task.position.z);
                glutSolidSphere(0.1, 5,5);
                glPopMatrix();
                glPushMatrix();
                // glColor3d(0.0, 0.0, 1.0);
                GLfloat facecolor4[] = {0.0,0.0,1.0,1.0};
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, facecolor4);
                glTranslated(task.position.x-move_x, task.position.y+move_y, task.position.z);
                glutSolidSphere(0.1, 5,5);
                glPopMatrix();
            }
            count++;
        }
        glDisable(GL_LIGHT1);
        glNormal3d(0.0, 0, 1);
        //display ground
        glPushMatrix();
        glTranslatef(0, 0, -9);
        glRotated(-82,1,0,0);
        // glColor3f(1.0, 1.0, 0.7); //cream
        GLfloat facecolor[] = {1.0,1.0,0.7,1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, facecolor);
        draw_rect2D_fill(3, 15.1);
        glTranslatef(-5, 0, 0);
        // glColor3f(0.0, 1.0, 0.0); //light green
        GLfloat facecolor1[] = {0.0,1.0,0.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor1);
        draw_rect2D_fill(7, 5);
        glTranslatef(10, 0, 0);
        // glColor3f(0.0, 1.0, 0.0); //light green
        draw_rect2D_fill(7, 5);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0, 0, -9.1);
        glRotated(-82,1,0,0);
        // glColor3f(0.0, 0.9, 0.0); //green foundation
        GLfloat facecolor2[] = {0.0,0.9,0.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor2);
        draw_rect2D_fill(20, 15);
        glPopMatrix();

        //display rock
        glPushMatrix();
        glTranslatef(-0.7,-1.6,3);
        GLfloat facecolor3[] = {0.6,0.6,0.6,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor3);
        // glColor3f(0.6, 0.6, 0.6); //grey
        glutSolidSphere(0.05, 5,5);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.5,-1.3,2);
        // glColor3f(0.6, 0.6, 0.6); //grey
        glutSolidSphere(0.05, 5,5);
        glPopMatrix();

        //display tree
        glPushMatrix();
        glRotated(-90,1,0,0);
        glTranslatef(-5,5,-0.3);//y->larger, further; z->up and down-3
        GLfloat facecolor4[] = {0.4,0.2,0.2,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor4);
        // glColor3d(0.4,0.2,0.2);//brown
        glutSolidCone(0.1,1.0,4.0,2.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(-5,0.4,-5.2);
        GLfloat facecolor5[] = {0.0,0.5,0.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor5);
        // glColor3d(0.0,0.5,0.0);//leaf green
        glutSolidSphere(0.5, 10,10);
        glPopMatrix();

        glPushMatrix();
        glRotated(-90,1,0,0);
        glTranslatef(4,-1,-1.3);//y->larger, further; z->up and down-3
        GLfloat facecolor6[] = {0.4,0.2,0.2,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor6);
        // glColor3d(0.4,0.2,0.2);//brown
        glutSolidCone(0.1,1.0,4.0,2.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(4,-0.6,0.8);
        GLfloat facecolor7[] = {0.0,0.5,0.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor7);
        // glColor3d(0.0,0.5,0.0);//leaf green
        glutSolidSphere(0.5, 10,10);
        glPopMatrix();

        glPushMatrix();
        glRotated(-90,1,0,0);
        glTranslatef(5,9,0.7);//y->larger, further; z->up and down-3
        GLfloat facecolor8[] = {0.4,0.2,0.2,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor8);
        // glColor3d(0.4,0.2,0.2);//brown
        glutSolidCone(0.1,1.0,4.0,2.0);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(5,1.4,-9.2);
        GLfloat facecolor9[] = {0.0,0.5,0.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor9);
        // glColor3d(0.0,0.5,0.0);//leaf green
        glutSolidSphere(0.5, 10,10);
        glPopMatrix();
        


        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);

        glFlush();

        //draw keyboard_input
        glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
        draw_string(keyboard_input, {100, 5});
        //draw sun
        glColor3d(1.0,0.0,0.0); //red
        if (time_limit<2*TIME_REDUCTION_SPEED){
            double per = time_limit*1.0/(2*TIME_REDUCTION_SPEED);
            glColor3d(0.05+0.95*per, 0.05*(1-per),0.3*(1-per));
        }
        Circle2DFill(13,20,180);//side, up and down
        if (time_limit/TIME_REDUCTION_SPEED%2)draw_around_sun(0);
        else draw_around_sun(16/M_PI);
        //draw time limit
        glColor3d(1.0,1.0,0.0);
        string str = to_string(time_limit/TIME_REDUCTION_SPEED);
        string moji = "TIME LIMITE";
        draw_string(moji, {10, 185});
        draw_string(str, {18, 175});
        //draw cloud
        glColor3d(1.0,1.0,1.0); //white
        if (time_limit<2*TIME_REDUCTION_SPEED){
            double per = time_limit*1.0/(2*TIME_REDUCTION_SPEED);
            glColor3d(0.05+0.95*per, 0.05+0.95*per,0.3+0.7*per);
        }
        Circle2DFill(8,175,190);
        Circle2DFill(8,185,190);
        Circle2DFill(8,190,183);
        Circle2DFill(8,180,180);
        Circle2DFill(8,170,183);
        //draw score
        glColor3d(0.2,0.3,1.0); 
        str = to_string(current_score);
        moji = "score";
        draw_string(moji, {175, 184});
        draw_string(str, {185, 184});
        //draw score effect
        glLineWidth(100);
        int r = 100-score_effect%100;
        xy message = {175,170+(double)r/3};
        if (score_effect/100==1){          
            moji = "good!";
            draw_string(moji, message);
            glColor3d(0.7,0.7,1.0);
            Circle2D(r,180,184);
        }else if (score_effect/100==2){
            moji = "great!";
            draw_string(moji, message);
            glColor3d(0.7,1.0,0.7); 
            Circle2D(r,180,184);
        }else if (score_effect/100==3){ 
            moji = "excellent!";
            draw_string(moji, message);
            glColor3d(1.0,0.7,0.7);
            Circle2D(r,180,184);
        }else if (score_effect/100==4){ 
            double l = 100-score_effect%100;
            moji = "bad";
            draw_string(moji, message);
            glColor3d(1.0,0.3,1.0);
            draw_line({180-l,184+l},{180+l,184-l});
            draw_line({180-l,184-l},{180+l,184+l});
        }
        glutSwapBuffers();
    }else if (g_display_mode==3){
        glLineWidth(1);
        glClearColor(0.05, 0.05,0.3, 0.2);
        glClear(GL_COLOR_BUFFER_BIT);
        //display current result
        glColor4f(1.0, 1.0, 0.0, 0.0);
        string moji = "result";
        draw_string(moji, {80,160});
        string str = to_string(past_score);
        draw_string(player_name, {95,160});
        draw_string(str, {115, 160});
        draw_line({78,155},{122,155});
        //display past data
        glColor4f(0.0, 1.0, 1.0, 0.0);
        double index = 0;
        for (auto &data: past_player){
            if (index==current_ranking){
                glColor4f(1.0, 1.0, 0.0, 0.0);
            }
            else glColor4f(0.0, 1.0, 1.0, 0.0);
            string num = to_string((int)index+1);
            draw_string(num,{80,130-index*10});
            draw_string(data.name, {95,130-index*10});
            string score = to_string(data.score);
            draw_string(score,{115,130-index*10});
            index++;
        }
        glColor4f(0.0, 1.0, 1.0, 0.0);
        moji = "ranking";
        draw_string(moji, {95, 140});
        draw_line({75,140},{93,140});
        draw_line({110,140},{125,140});
        draw_line({75,140},{75,135-index*10});
        draw_line({75,135-index*10},{125,135-index*10});
        draw_line({125,140},{125,135-index*10});
        //display instruction
        glColor3d(0.0,0.2,0.4);
        moji = "press 2 to go home";
        double x_pos = 88;
        draw_string(moji, {x_pos, 50});
        moji = "press 1 to continue";
        draw_string(moji, {x_pos+2, 40});
        moji = "press esc to exit";
        draw_string(moji, {x_pos+4, 30});
        //if the score was best
        glColor4f(1.0, 1.0, 0.0, 0.0);
        if (current_ranking==0){
            moji = "Congratulations!";
            draw_string(moji, {150, 120});
            moji = "Best Score!";
            draw_string(moji, {30, 120});
            double dif=0;
            if (ordinary_timer/15%2)dif = M_PI/6;
            double r = 2.5-dif;
            draw_triangle({55,135}, r, dif);
            draw_triangle({55,135}, r, M_PI/3+dif);
            draw_triangle({25,105}, r, M_PI/6+dif);
            draw_triangle({25,105}, r, -M_PI/6+dif);
            draw_triangle({145,135}, r, dif);
            draw_triangle({145,135}, r, M_PI/3+dif);
            draw_triangle({175,105}, r, M_PI/6+dif);
            draw_triangle({175,105}, r, -M_PI/6+dif);
        }else if (current_ranking<5){
            moji = "Good job!";
            draw_string(moji, {150, 120});
            moji = "Rank in!";
            draw_string(moji, {30, 120});
            double dif=0;
            if (ordinary_timer/15%2)dif = M_PI/6;
            double r = 1.5;
            draw_triangle({25,105}, r, M_PI/6+dif);
            draw_triangle({25,105}, r, -M_PI/6+dif);
            draw_triangle({175,135}, r, dif);
            draw_triangle({175,135}, r, M_PI/3+dif);
        }
        glutSwapBuffers();
        glFlush();
        
    }  
}

void draw_task(string text){
    double edge_x = 0.809;
    double edge_y = 0.5;
    double edge_z = 0.3;
    //draw text
    double plate_z = edge_z*1.01;//a little bit formaer than rectangle
    double plate_width = 0.1;
    for (int i=0; i<(int)text.size(); i++){
        glBindTexture(GL_TEXTURE_2D, g_TextureHandles_lower_letter[text[i]-'a']);
        double plate_x_l = -edge_x*0.9+i*plate_width;
        double plate_x_r = -edge_x*0.9+(i+1)*plate_width;
        double plate_y = edge_y*0.8;
        GLdouble pointA[] = {plate_x_r, plate_y, plate_z};//width, height, depth
        GLdouble pointB[] = {plate_x_l, plate_y, plate_z};
        GLdouble pointC[] = {plate_x_l, -plate_y, plate_z};
        GLdouble pointD[] = {plate_x_r, -plate_y, plate_z};
        glColor3d(1.0, 1.0, 1.0);

        glEnable(GL_TEXTURE_2D);
        GLfloat facecolor[] = {1.0,1.0,1.0,1.0};
        glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
        glNormal3d(0.0, 0.0, 1);
        glBegin(GL_POLYGON);
        glTexCoord2d(1.0, 0.0);
        glVertex3dv(pointA);
        glTexCoord2d(0.0, 0.0);
        glVertex3dv(pointB);
        glTexCoord2d(0.0, 1.0);
        glVertex3dv(pointC);
        glTexCoord2d(1.0, 1.0);
        glVertex3dv(pointD);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }


    //draw rectangle
    vector<vector<int>> rect_edge = {{0,1,2,3},{0,4,5,1},{1,5,6,2},{2,6,7,3},{0,4,7,3},{4,5,6,7}};//vertexes of each surface
    vector<xyz> code = {{1,1,1},{1,1,-1},{-1,1,-1},{-1,1,1},{1,-1,1},{1,-1,-1},{-1,-1,-1},{-1,-1,1}};//code of each vertex
    vector<xyz> n_vec = {{0,1,0},{1,0,0},{0,0,-1},{-1,0,0},{0,0,1},{0,-1,0}};
    for (int i=0; i<6; i++){
        glNormal3d(n_vec[i].x, n_vec[i].y, n_vec[i].z);
        if (i==4){
            glDisable(GL_LIGHT0);
            // if (text.size()<6)glColor3d(0.7,0.7,1.0);
            // else if (text.size()<10)glColor3d(0.7,1.0,0.7);
            // else glColor3d(1.0, 0.7, 0.7);//pink//green//blue
            if (text.size()<6){
                GLfloat facecolor[] = {0.7,0.7,1.0,1.0};//blue
                glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
            }else if (text.size()<10){
                GLfloat facecolor[] = {0.7,1.0,0.7,1.0};//green
                glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
            }else{
                GLfloat facecolor[] = {1.0,0.7,0.7,1.0};//pink
                glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
            }
        }else{
            GLfloat facecolor[] = {0.4,0.2,0,1.0};//brown
            glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
        }
        // else glColor3d(0.5, 0.5, 0);//brown
        glBegin(GL_POLYGON);
        for (int j=0; j<4; j++){
            GLdouble point[] = {edge_x*code[rect_edge[i][j]].x, edge_y*code[rect_edge[i][j]].y, edge_z*code[rect_edge[i][j]].z};
            glVertex3dv(point);
        }
        glEnd();
        if (i==4){
            glEnable(GL_LIGHT0);
        }
    }
}

void draw_rect2D_fill(double x, double y){
    glBegin(GL_POLYGON);
    for (int i=0; i<4; i++){
        GLdouble point[] = {x*rect_2D_code[i].x,y*rect_2D_code[i].y,0};
        glVertex3dv(point);
    }
    glEnd();
}

void draw_rect2D_line(double x, double y){
    glBegin(GL_LINE_LOOP);
	for (int i=0; i<4; i++){
        GLdouble point[] = {x*rect_2D_code[i].x,y*rect_2D_code[i].y,0};
        glVertex3dv(point);
    }
	glEnd();
}

void Circle2DFill(float radius,int x,int y)
{
 for (float th1 = 0.0; th1 <= 360.0; th1 = th1 + 1.0)
 {       
 float th2 = th1 + 10.0;
 float th1_rad = th1 / 180.0 * M_PI; 
 float th2_rad = th2 / 180.0 * M_PI;
 float x1 = radius * cos(th1_rad);
 float y1 = radius * sin(th1_rad);
 float x2 = radius * cos(th2_rad);
 float y2 = radius * sin(th2_rad);
 glBegin(GL_TRIANGLES); 
  glVertex2f( x, y );
  glVertex2f( x1+x, y1+y );   
  glVertex2f( x2+x, y2+y );
 glEnd();
 } 
}

void draw_around_sun(double s_rad){
    double num = 8;
    double r_in = 15;
    double r_out = 20;
    double center_x = 20;
    double center_y = 180;
    double dif = 0.1;
    xy point1,point2,point3;
    for (double i=0; i<num; i++){
        glBegin(GL_TRIANGLES);
        point1.x = center_x+r_in*cos(i*M_PI/4+s_rad);
        point1.y = center_y+r_in*sin(i*M_PI/4+s_rad);
        glVertex2d(point1.x,point1.y);
        point2.x = center_x+r_out*cos(i*M_PI/4+dif+s_rad);
        point2.y = center_y+r_out*sin(i*M_PI/4+dif+s_rad);
        glVertex2d(point2.x,point2.y);
        point3.x = center_x+r_out*cos(i*M_PI/4-dif+s_rad);
        point3.y = center_y+r_out*sin(i*M_PI/4-dif+s_rad);
        glVertex2d(point3.x,point3.y);
        glEnd();
    }
}

void draw_string(string &text, xy position){
    glPushAttrib(GL_ENABLE_BIT);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 200, 0, 200);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glRasterPos2d(position.x, position.y);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24 , reinterpret_cast<const unsigned char*> (text.c_str()));
}

void draw_line(xy a, xy b){
    glBegin(GL_LINES);
    glVertex2d(a.x,a.y);
    glVertex2d(b.x,b.y);
    glEnd();
}

void draw_triangle(xy center, double r, double s_rad){
    xy point1,point2,point3;
    glBegin(GL_TRIANGLES);
    point1.x = center.x+r*cos(s_rad);
    point1.y = center.y+r*sin(s_rad);
    glVertex2d(point1.x,point1.y);
    point2.x = center.x+r*cos(2*M_PI/3+s_rad);
    point2.y = center.y+r*sin(2*M_PI/3+s_rad);
    glVertex2d(point2.x,point2.y);
    point3.x = center.x+r*cos(4*M_PI/3+s_rad);
    point3.y = center.y+r*sin(4*M_PI/3+s_rad);
    glVertex2d(point3.x,point3.y);
    glEnd();
}

void Circle2D(float radius,int x,int y)
{
 for (float th1 = 0.0; th1 <= 360.0; th1 = th1 + 1.0)
 {       
 float th2 = th1 + 10.0;
 float th1_rad = th1 / 180.0 * M_PI; 
 float th2_rad = th2 / 180.0 * M_PI;
 float x1 = radius * cos(th1_rad);
 float y1 = radius * sin(th1_rad);
 float x2 = radius * cos(th2_rad);
 float y2 = radius * sin(th2_rad);
 glBegin(GL_LINES);  
  glVertex2f( x1+x, y1+y );   
  glVertex2f( x2+x, y2+y );
 glEnd();
 }
}