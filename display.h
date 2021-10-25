#include <string>
#include <vector>
#include <GL/glut.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#define TIME_REDUCTION_SPEED (20)
#define NUMBER_OF_ALPHABET (27)
#define NUMBER_OF_NUMBERS (10)
struct xy{double x; double y;};
struct xyz{double x; double y; double z;};
struct task_data{int num; std::string title; xyz position; int status;}; //statue=0 -> not clear, status=1 -> clear
struct player{std::string name; int score;};

void glut_display();
extern int g_display_mode;
extern std::vector<task_data> task_image_data;
extern std::string keyboard_input;
extern int time_limit;
extern int current_score;
extern int current_ranking;
extern int past_score;
extern GLuint g_TextureHandles_upper_letter[NUMBER_OF_ALPHABET];
extern std::vector<xy> rect_2D_code;
extern GLuint g_TextureHandles_lower_letter[NUMBER_OF_ALPHABET];
extern GLuint g_TextureHandles_number[NUMBER_OF_NUMBERS];
extern std::string player_name;
extern std::vector<player> past_player;
extern int ordinary_timer;
extern int score_effect;