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
#include <algorithm>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "display.h"
using namespace std;
struct original_data{int num; string title; bool available;};

#define WINDOW_X (1500)
#define WINDOW_Y (1000)
#define WINDOW_NAME "gaming screen"


void read_task_file(const string& file_name);
void read_score_saving_file(const string& file_name);
void init_GL(int argc, char *argv[]);
void init();
void set_callback_functions();
void glut_keyboard(unsigned char key, int x, int y);
void set_texture();
void set_task_init();
void display();
void timer(int value);
void aggregate_score();
void write_score_saving_file();
void score_init();

// グローバル変数
int time_limit_second = 30;
vector<original_data> original_image_data;
vector<task_data> task_image_data;
int timer_set_time = 50;// the smaller, the faster the task moves
int current_score = 0;
int past_score = 0;
int current_ranking;//0~
int ordinary_timer = 0;
int time_limit = time_limit_second*TIME_REDUCTION_SPEED;
int score_effect = 0;//~199 good, ~299 great, ~399 excellent, ~499 bad
int effect_period=10;//score_effect-effect_period is when the effect disappear
vector<xy> rect_2D_code = {{1,1},{-1,1},{-1,-1},{1,-1}};
int g_display_mode = 1;//1: player register,  2: playing, 3: result
GLuint g_TextureHandles_upper_letter[NUMBER_OF_ALPHABET];
GLuint g_TextureHandles_lower_letter[NUMBER_OF_ALPHABET];
GLuint g_TextureHandles_number[NUMBER_OF_NUMBERS];
string keyboard_input;
xyz task_initial_position = {2.0,2.0,-20.0};//-11
double task_disappear_position_z = 7.0;
vector<int> score_array = {0,100,100,100,100,100,200,200,200,200,300};
string player_name;
vector<player> past_player;

int main(int argc, char *argv[]){
    /* read task_string */
    read_task_file("task_string.txt");
    read_score_saving_file("score_saving.txt");
    cv::namedWindow("image",1);//delete later

	/* OpenGLの初期化 */
	init_GL(argc, argv);

	/* このプログラム特有の初期化 */
	init();

	/* コールバック関数の登録 */
	set_callback_functions();

	/* メインループ */
	glutMainLoop();                              // 無限ループ。コールバック関数が呼ばれるまでずっと実行される。

	return 0;
}

void read_task_file(const string& file_name){
    std::ifstream file(file_name);
    if (!file){
        std::cerr << "error: file not found" <<endl;
        std::exit(1);
    }
    string line;
    int count = 0;
    while(getline(file, line)){
        original_image_data.push_back({count, line, true});
        count++;
    }
    if (count<6){
        std::cerr << "error: the number of words is not enough" <<endl;
        std::exit(1);
    }
}

void read_score_saving_file(const string& file_name){
    std::ifstream file(file_name);
    if (!file){
        std::cerr << "error: file not found" <<endl;
        std::exit(1);
    }
    string line;
    for (int row=0; getline(file, line); row++){
        istringstream stream(line);
        string name; stream>>name;
        int score; stream>>score;
        past_player.push_back({name,score});
    }
}

void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);                              // OpenGLの初期化
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);          // ディスプレイモードをRGBAモードに設定
	glutInitWindowSize(WINDOW_X, WINDOW_Y);                              // ウィンドウサイズを指定
	glutCreateWindow(WINDOW_NAME);                              // ウィンドウを「生成」。まだ「表示」はされない。
}

void init(){
	glClearColor(0.4f, 0.9f, 0.9f, 0.2f);                              // 背景の塗りつぶし色を指定->light blue
    glGenTextures(NUMBER_OF_ALPHABET, g_TextureHandles_lower_letter);
    set_texture();
    set_task_init();
    srand(time(NULL));
}

void set_callback_functions(){
	glutDisplayFunc(glut_display);                              // ディスプレイに変化があった時に呼ばれるコールバック関数を登録
	glutKeyboardFunc(glut_keyboard);                              // キーボードに変化があった時に呼び出されるコールバック関数を登録
    glutTimerFunc(timer_set_time, timer, 0);
}

// キーボードに変化があった時に呼び出されるコールバック関数。
void glut_keyboard(unsigned char key, int x, int y){
    if (g_display_mode==1){
        switch(key){
            case '\r':
                g_display_mode=2;
                break;
            case '\033':
                exit(0);
                break;
            case '\b': //delete key
                if (player_name.size()) player_name.pop_back();
                break;
            default:
                player_name.push_back(key);
        }
    }else if (g_display_mode==2){
        bool input_was_right = false;
        switch(key){
            case '\r':
                for (auto& task: task_image_data){
                    if (task.title==keyboard_input&&task.status==0) {
                        task.status = 1;
                        current_score+=score_array.at(min((int)task.title.size(),10));//change_score
                        score_effect=score_array.at(min((int)task.title.size(),10))+99;
                        input_was_right = true;
                    }
                }
                if (input_was_right==false){
                    current_score = max(current_score-50,0);//change_score
                    score_effect = 499;
                }
                keyboard_input.clear();
                break;
            case '\033': // Escキーのこと
                exit(0);
                break;
            case '\b': //delete key
                if (keyboard_input.size()) keyboard_input.pop_back();
                break;
            default:
                keyboard_input.push_back(key);
        }
    }else{
        switch(key){
            case '2':
                g_display_mode=1;
                player_name.clear();
                break;
            case '\033':
                exit(0);
                break;
            case '1':
                g_display_mode=1;
                break;
        }
    }
    

	glutPostRedisplay();                  // 「ディスプレイのコールバック関数を呼んで」と指示する。
}

void set_texture(){
    double texture_width; double texture_height;

	for(int i = 0; i < 26; i++){
        string num = to_string(i);
        cv::Mat character = cv::imread("text_image/"+num+".jpg", 1);
        if(character.empty()){
            std::cerr << "error: file not found" <<endl;
            std::exit(1);
        }
        texture_width = character.size().width;
        texture_height = character.size().height;
        glBindTexture(GL_TEXTURE_2D, g_TextureHandles_lower_letter[i]);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// glBindTexture(GL_TEXTURE_2D, g_TextureHandles[i]);
		glTexSubImage2D(GL_TEXTURE_2D, 0,
			(texture_width - character.cols) / 2,
			(texture_height - character.rows) / 2,
			character.cols, character.rows,
			GL_RGB, GL_UNSIGNED_BYTE, character.data);
	}
}

void set_task_init(){
    score_effect=0;
    task_image_data.clear();
    for (auto &data: original_image_data){
        if (data.available==false)data.available=true;
    }
    int num = (int)rand()%original_image_data.size();
    task_image_data.push_back({num, original_image_data[num].title, {task_initial_position.x,task_initial_position.y,task_initial_position.z}, 0});
    original_image_data[num].available = false;
    while(original_image_data[num].available == false){
        num = (int)rand()%original_image_data.size();
    }
    task_image_data.push_back({num, original_image_data[num].title, {-task_initial_position.x,task_initial_position.y,task_initial_position.z}, 0});
    original_image_data[num].available = false;
}
void timer(int value){
    if (g_display_mode!=2){
        ordinary_timer++;
        if (ordinary_timer>INT_MAX-3)ordinary_timer=0;
        glutPostRedisplay();                  // 「ディスプレイのコールバック関数を呼んで」と指示する。
        glutTimerFunc(timer_set_time,timer,0);
        return;
    }
    if (time_limit>=0)time_limit--; //timer reduce
    if (score_effect%100>100-effect_period)score_effect--;
    else score_effect=0;
    if (task_image_data.size()<6){ //At beginning, there are fewer tasks
        for (auto task: task_image_data){
            // if (task.position.z<-3.95&&-4.05<task.position.z){ //new task add
            double task_new_add_position = task_initial_position.z*2/3+task_disappear_position_z/3;
            if (task_new_add_position-0.05<task.position.z&&task.position.z<task_new_add_position+0.05){ //new task add
                int num = (int)rand()%original_image_data.size();
                while(original_image_data[num].available == false){
                    num = (int)rand()%original_image_data.size();
                }
                task_image_data.push_back({num, original_image_data[num].title, {task_initial_position.x,task_initial_position.y,task_initial_position.z}, 0});
                original_image_data[num].available = false;
                while(original_image_data[num].available == false){
                    num = (int)rand()%original_image_data.size();
                }
                task_image_data.push_back({num, original_image_data[num].title, {-task_initial_position.x,task_initial_position.y,task_initial_position.z}, 0});
                original_image_data[num].available = false;
                break;
            }
        }
    }
    double dy = 0.02;
    double dz = (task_disappear_position_z-task_initial_position.z)/180;
    //move task position
    for (auto& task: task_image_data){
        task.position.z+=dz;
        if (task.status)task.status++;//completed task status change
        else task.position.y-=dy;//if task is displayed, decrease y
        if (task.position.z>task_disappear_position_z){//completed task disappear. Newer task added
            original_image_data[task.num].available = true;
            int num = (int)rand()%original_image_data.size();
            while(original_image_data[num].available == false){
                num = (int)rand()%original_image_data.size();
            }
            task.num = num;
            task.title = original_image_data[num].title;
            task.position.y = task_initial_position.y;
            task.position.z = task_initial_position.z;
            task.status = 0;
            original_image_data[num].available = false;
        }
    }
    if (time_limit<0&&g_display_mode==2){
        g_display_mode=3;
        aggregate_score();
        score_init();
    }
    glutPostRedisplay();                  // 「ディスプレイのコールバック関数を呼んで」と指示する。
    glutTimerFunc(timer_set_time,timer,0);
}

void aggregate_score(){
    past_player.push_back({player_name,current_score});
    sort(past_player.begin(),past_player.end(),[](const player &x, const player &y){
        return x.score > y.score;
    });
    if (past_player.size()>5)past_player.pop_back();
    write_score_saving_file();
    for (int i=0; i<past_player.size(); i++){
        if (past_player[i].name==player_name&&past_player[i].score==current_score){
            current_ranking=i;
            break;
        }else{
            current_ranking=5;
        }
    }
}

void write_score_saving_file(){
    ofstream file("score_saving.txt");
    if (!file){
        std::cerr << "error: file not found" <<endl;
        std::exit(1);
    }
    for (auto data: past_player){
        file<<data.name<<" "<<data.score<<endl;
    }
}

void score_init(){
    time_limit = time_limit_second*TIME_REDUCTION_SPEED;
    past_score = current_score;
    current_score = 0;
    set_task_init();
}