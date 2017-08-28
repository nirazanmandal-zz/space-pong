//#include<stdlib.h>
//#include<stdio.h>
#include<SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include<stdbool.h>
#include<time.h>

//The screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_SIZE = 500;

//The frame rate
const int FRAMES_PER_SECOND = 15;

int BALL_MAX_SPEED = 4;
int PADDLE1_SPEED = 4;
int PADDLE2_SPEED = 2;
int BALL_X_SPEED = 4;

//create surfaces
static SDL_Surface *screen=NULL;
static SDL_Surface *paddle1=NULL;
static SDL_Surface *paddle2=NULL;
static SDL_Surface *ball=NULL;
static SDL_Surface *temp=NULL;
static SDL_Surface *bg = NULL;
static SDL_Surface *menu = NULL;
static SDL_Surface *help = NULL;
static SDL_Surface *end = NULL;
SDL_Surface *P1score_surface;
SDL_Surface *P2score_surface;

SDL_Rect rcpaddle1,rcpaddle2;
SDL_Rect BallRect,titleRect;
SDL_Rect rcbg,rcmenu,end_pos,help_pos;
SDL_Rect p1score_pos,p2score_pos;
SDL_Rect comp,p1,p2, dest;

//sound
Mix_Music *sound1 = NULL;
Mix_Music *sound2 = NULL;
Mix_Music *sound3 = NULL;
Mix_Music *sound4 = NULL;
Mix_Music *sound5 = NULL;
Mix_Music *sound6 = NULL;

//sounds that will be used
Mix_Chunk *ball_hit_paddle = NULL;
Mix_Chunk *ballout = NULL;
Mix_Chunk *shot = NULL;
Mix_Chunk *bg_music = NULL;
Mix_Chunk *p1wins = NULL;
Mix_Chunk *p2wins = NULL;

//font
TTF_Font *font = NULL;

//Text Color
SDL_Color foregroundColor = { 0, 0, 255 };
SDL_Color backgroundColor = { 255, 255, 255 };
SDL_Color textcolor = {255, 255, 255};

SDL_Event event;
Uint8 *keystate;

int colorkey;
int gameover;
int state=0;
int r = 0;

char player1HUD[20];
char player2HUD[20];

//score
int p1_score=0;
int p2_score=0;

//ball speed
int BallXVel=1;
int BallYVel=1;
int speed1;

//check collision
bool check_collision(SDL_Rect A, SDL_Rect B)
{

    //sides of rectangle
    int leftA, leftB;
    int rightA, rightB;
    int topA,topB;
    int bottomA, bottomB;

    //calculate sides of rectangle A
    leftA=A.x;
    rightA=A.x+A.w;
    topA=A.y;
    bottomA=A.y+A.h;

    //calculate the sides of rectangle B
    leftB=B.x;
    rightB=B.x+B.w;
    topB=B.y;
    bottomB=B.y+B.h;

    //if any sides from A are outsides of B
    if(bottomA<=topB)
    {
        return false;
    }
    if(topA>=bottomB)
    {
        return false;
    }
    if(rightA<=leftB)
    {
        return false;
    }
    if(leftA>=rightB)
    {
        return false;
    }
    //if non of the sides from A are outside B
    return true;

}

//initial location of items
void ResetGame()
{
    //locarion of paddle 1
    rcpaddle1.x = screen->w/2 -130;
    rcpaddle1.y = 30;

    //location of paddle 2
    rcpaddle2.x = screen->w/2 -130;
    rcpaddle2.y = 690;

    //location of ball
    BallRect.x = 200;
    BallRect.y = 410;

    //velocity of ball
    BallXVel=1;
    BallYVel=1;


    //text=NULL;

    //score location
    p1score_pos.x = 100;
    p1score_pos.y = 600;

    p2score_pos.x=100;
    p2score_pos.y=100;

}

//check score AI vs Player
void check_score_ai_player()
{
    if(p1_score==6 || p2_score==6)
    {
	    Mix_PauseMusic();
		if(p2_score > p1_score)
		{
            draw_game_over(3);
		}
		else
		{
             draw_game_over(1);
		}
        p1_score=0;
        p2_score=0;
    }
    SDL_Delay(1000);
}

//check score player vs player
void check_score_player_player()
{
    if(p1_score==6 || p2_score==6)
    {
	    Mix_PauseMusic();
		if(p2_score > p1_score)
		{
            draw_game_over(2);
		}
		else
		{
             draw_game_over(1);
		}
        p1_score=0;
        p2_score=0;
    }
    SDL_Delay(1000);
}

//AI vs AI
void ai_vs_ai()
{

    //move AI paddle1
    if((rcpaddle1.x+rcpaddle1.w/2) > (BallRect.x))
        rcpaddle1.x -= 1;

    if((rcpaddle1.x+rcpaddle1.w/2) < (BallRect.x))
        rcpaddle1.x += 1;

    if(rcpaddle1.x<0)
    {
        rcpaddle1.x=0;
    }
    else if(rcpaddle1.x>SCREEN_WIDTH+SCREEN_SIZE)
    {
        rcpaddle1.x=SCREEN_WIDTH+SCREEN_SIZE;
    }




    //move AI paddle2
    if((rcpaddle2.x+rcpaddle2.w/2) > (BallRect.x))
        rcpaddle2.x -= 1;

    if((rcpaddle2.x+rcpaddle2.w/2) < (BallRect.x))
        rcpaddle2.x += 1;

    if(rcpaddle2.x<0)
    {
        rcpaddle2.x=0;
    }
    else if(rcpaddle2.x>SCREEN_WIDTH+SCREEN_SIZE)
    {
        rcpaddle2.x=SCREEN_WIDTH+SCREEN_SIZE;
    }

    //move ball
    BallRect.x += BallXVel;
    BallRect.y += BallYVel;

    //Turn the ball around if it collides with the edge of the screen.
    if(BallRect.x>1340)
    {
        BallXVel= -1 ;
    }
    if (BallRect.x<0)
    {
        BallXVel = + 1;
    }
    if(BallRect.y<0)
    {
        BallXVel=1;
        BallYVel=1;
        ResetGame();
    }
    if(BallRect.y>740)
    {
        BallXVel=1;
        BallYVel=1;
        ResetGame();
    }

    int speed1=rand()%BALL_MAX_SPEED +1;
    int speed2=rand()%BALL_MAX_SPEED +1;

    //collision with paddle 1
    if(check_collision(rcpaddle1, BallRect))
    {
        BallYVel=speed1;
    }
    //collision with paddle 2
    else if(check_collision(rcpaddle2, BallRect))
    {
        BallYVel=-speed2;
    }
}


//Move ball when playing player vs AI
void player_vs_ai()
{
    //move ai paddle
    if((rcpaddle1.x+rcpaddle1.w/2) > (BallRect.x+BallRect.w/2))
        rcpaddle1.x -= rand()% PADDLE1_SPEED +1;

    if((rcpaddle1.x+rcpaddle1.w/2) < (BallRect.x+BallRect.w/2))
        rcpaddle1.x += rand()% PADDLE1_SPEED + 1;

    //collision with edges of screen
    if(rcpaddle1.x<0)
    {
        rcpaddle1.x=0;
    }
    else if(rcpaddle1.x>SCREEN_WIDTH+SCREEN_SIZE)
    {
        rcpaddle1.x=SCREEN_WIDTH+SCREEN_SIZE;
    }

    // move player paddle
    keystate=SDL_GetKeyState(NULL);

    if(keystate[SDLK_KP4])
    {
        rcpaddle2.x-=PADDLE2_SPEED;
    }
    if(keystate[SDLK_KP6])
    {
        rcpaddle2.x+=PADDLE2_SPEED;
    }

    //collide with edges of screen
    if ( rcpaddle2.x < 0 )
    {
        rcpaddle2.x= 0;
    }
    else if ( rcpaddle2.x>SCREEN_WIDTH+SCREEN_SIZE )
    {
        rcpaddle2.x = SCREEN_WIDTH+SCREEN_SIZE;
    }

    //move ball
    BallRect.x += BallXVel;
    BallRect.y += BallYVel;

    //Turn the ball around if it collides with the edge of the screen.
    if(BallRect.x>1340)
    {
        BallXVel= -1 ;
    }
    if (BallRect.x<0)
    {
        BallXVel = + 1;
    }

    //if the ball leaves the screen from up
    if(BallRect.y<0)
    {
        Mix_PlayMusic(sound2,0);
        p1_score=p1_score+1;

        check_score_ai_player();

        ResetGame();

    }

    //if the ball leaves the screen from down
    if(BallRect.y>740)
    {
        Mix_PlayMusic(sound2,0);
        p2_score=p2_score+1;

        check_score_ai_player();

        ResetGame();
    }

    //random paddle speed
    speed1=rand()%BALL_MAX_SPEED +1;
    //PADDLE2_SPEED=speed1;

    //collide with paddle 1
    if(check_collision(rcpaddle1, BallRect))
    {
        BallYVel= speed1;
        if (BallXVel==0)
        {
            BallXVel -= (rand()% BALL_X_SPEED)+1;
        }

        if( BallYVel== 4)
        {
            Mix_PlayMusic(sound3,0);
            if(rcpaddle2.x > screen->w/2 -130)
            {
                BallXVel -= (rand()% BALL_X_SPEED)+1;
            }
            else
            {
                BallXVel = (rand()% BALL_X_SPEED)+1;
            }
        }
        else if( BallYVel== 3)
        {
             Mix_PlayMusic(sound1,0);
        }
        else
        {
             Mix_PlayMusic(sound1,0);
        }
    }

    //collide with paddle 2
    else if(check_collision(rcpaddle2, BallRect))
    {
        keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_KP8])
        {
            Mix_PlayMusic(sound3,0);
            BallYVel = - 6;
        }

        else if (keystate[SDLK_KP7])
            {
                Mix_PlayMusic(sound3,0);
                BallYVel = - 4;
                BallXVel -= (rand()% BALL_X_SPEED)+1;
            }
       else if (keystate[SDLK_KP9])
            {
                Mix_PlayMusic(sound3,0);
                BallYVel = - 4;
                BallXVel = (rand()% BALL_X_SPEED)+1;
            }

        else
        {
            Mix_PlayMusic(sound1,0);
            BallYVel = - 2;
        }

    }
}

//player vs player
void player_vs_player()
{

    // move player paddle 1
    keystate=SDL_GetKeyState(NULL);
    if(keystate[SDLK_a])
    {
        rcpaddle1.x-=2;
    }
    if(keystate[SDLK_d])
    {
        rcpaddle1.x+=2;
    }
    //collide with edges of screen
    if(rcpaddle1.x<0)
    {
        rcpaddle1.x=0;
    }
    else if(rcpaddle1.x>SCREEN_WIDTH+SCREEN_SIZE)
    {
        rcpaddle1.x=SCREEN_WIDTH+SCREEN_SIZE;
    }
    if(!paddle1 && !paddle2)
    {
        printf("Image loaded incorrectly");
    }

    // move player paddle 2
    keystate=SDL_GetKeyState(NULL);
    if(keystate[SDLK_KP4])
    {
        rcpaddle2.x-=2;
    }
    if(keystate[SDLK_KP6])
    {
        rcpaddle2.x+=2;
    }

    //collide with edges of screen
    if ( rcpaddle2.x < 0 )
    {
        rcpaddle2.x= 0;
    }
    else if ( rcpaddle2.x>SCREEN_WIDTH+SCREEN_SIZE )
    {
        rcpaddle2.x = SCREEN_WIDTH+SCREEN_SIZE;
    }

    //move ball
    int ball_speed;
    BallRect.x += BallXVel;
    BallRect.y += BallYVel;

    //Turn the ball around if it collides with the edge of the screen.
    if(BallRect.x>1340)
    {
        BallXVel= -1 ;
    }
    if (BallRect.x<0)
    {
        BallXVel = + 1;
    }
    if(BallRect.y<0)
    {
        Mix_PlayMusic(sound2,0);
        p1_score=p1_score+1;

        check_score_player_player();

        BallXVel=1;
        BallYVel=1;
        ResetGame();
    }
    if(BallRect.y>740)
    {
        Mix_PlayMusic(sound2,0);
        p2_score=p2_score+1;

        check_score_player_player();

        BallXVel=1;
        BallYVel=1;
        ResetGame();
    }

    //collide with paddle 1
    if(check_collision(rcpaddle1, BallRect))
    {
        keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_w])
        {
            Mix_PlayMusic(sound3,0);
            BallYVel = 6;

        }
        else if(keystate[SDLK_e])
        {
            Mix_PlayMusic(sound3,0);
            BallYVel = 4;
            BallXVel = (rand()% BALL_X_SPEED)+1;
        }
        else if (keystate[SDLK_q])
        {
            Mix_PlayMusic(sound3,0);
            BallYVel = 4;
            BallXVel -= (rand()% BALL_X_SPEED)+1;
        }
        else
        {
            Mix_PlayMusic(sound1,0);
            BallYVel =  2;
        }
    }

    //collide with paddle 2
    else if(check_collision(rcpaddle2, BallRect))
    {
        //int speed2 = rand()%BALL_MAX_SPEED +1;
        keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_KP8])
        {
            Mix_PlayMusic(sound3,0);
            BallYVel = - 6;
        }

        else if (keystate[SDLK_KP7])
            {
                Mix_PlayMusic(sound3,0);
                BallYVel = - 4;
                BallXVel -= (rand()% BALL_X_SPEED)+1;
            }
       else if (keystate[SDLK_KP9])
            {
                Mix_PlayMusic(sound3,0);
                BallYVel = - 4;
                BallXVel = (rand()% BALL_X_SPEED)+1;
            }

        else
        {
            Mix_PlayMusic(sound1,0);
            BallYVel = - 2;
        }

    }
}

//initialize sdl, font, audio
int initialize()
{
     // initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // set the title bar
    SDL_WM_SetCaption("PING PONG", "PING PONG");
    SDL_WM_SetIcon(SDL_LoadBMP("images/icon.bmp"), NULL);

    // create window;
    screen = SDL_SetVideoMode(1366 ,760 , 32, SDL_FULLSCREEN);

    //Initialize Font
    TTF_Init();

    //Initialize sound
    Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 1024 );

}

// load menu
void select()
{
    //load menu
    temp=SDL_LoadBMP("images/pong_menu.bmp");
    menu = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
}

//load background
void background()
{
    //load background
    temp=SDL_LoadBMP("images/back.bmp");
    bg = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
}

//load gameover
void load_end()
{
    temp=SDL_LoadBMP("images/gameover.bmp");
    end = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);
}


//load files
int load_files()
{
    // load paddle1
    temp=SDL_LoadBMP("images/paddle_up.bmp");
    paddle1 = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

    colorkey = SDL_MapRGB(paddle1->format, 255, 255, 0);
    SDL_SetColorKey(paddle1, SDL_SRCCOLORKEY, colorkey);

    // load paddle2
    temp=SDL_LoadBMP("images/paddle_down.bmp");
    paddle2 = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

    //load ball
    temp=SDL_LoadBMP("images/ball.bmp");
    ball = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	colorkey = SDL_MapRGB(ball->format, 255, 255, 255);
    SDL_SetColorKey(ball, SDL_SRCCOLORKEY, colorkey);

    // load help
    temp=SDL_LoadBMP("images/help.bmp");
    help = SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);


    //load sounds
    sound1 = Mix_LoadMUS("sounds/ball_hit_paddle.ogg");
    sound2 = Mix_LoadMUS("sounds/ballout.ogg");
    sound3 = Mix_LoadMUS("sounds/shot.ogg");
    sound4 = Mix_LoadMUS("sounds/alien.mp3");
    sound5 = Mix_LoadMUS("sounds/p1wins.mp3");
    sound6 = Mix_LoadMUS("sounds/p2wins.mp3");


    //load font
    font = TTF_OpenFont("snap.ttf",30);

    return 0;

}

//draw paddle and background
void blit_surface()
{
    //SDL_BlitSurface(bg, NULL, screen, &rcbg);
    //SDL_BlitSurface(over, NULL, screen, &over_pos);
    SDL_BlitSurface(paddle1, NULL, screen, &rcpaddle1);
    SDL_BlitSurface(paddle2, NULL, screen, &rcpaddle2);
    SDL_BlitSurface(ball, NULL, screen, &BallRect);
    SDL_BlitSurface(P1score_surface, NULL, screen, &p1score_pos);
    SDL_BlitSurface(P2score_surface, NULL, screen, &p2score_pos);
}

void clean_up()
{
    SDL_FreeSurface(paddle1);
    SDL_FreeSurface(paddle2);
    SDL_FreeSurface(ball);
    SDL_FreeSurface(menu);
    SDL_FreeSurface(bg);
    SDL_FreeSurface(end);
    //SDL_FreeSurface(P1score_surface);
    //SDL_FreeSurface(P2score_surface);

    //Free Music
 	Mix_FreeMusic(sound1);
 	Mix_FreeMusic(sound2);
 	Mix_FreeMusic(sound3);
 	Mix_FreeMusic(sound4);

 	//Quit Audio
    Mix_CloseAudio();

    //Quit Font
	TTF_CloseFont(font);

	//Quit SDL_ttf
	TTF_Quit();
}

void ai_menu()
{
    if(state==0)
    {
        select();
        load_files();
        Mix_PlayMusic(sound4,5);
        ResetGame();
    }

    while (state == 0 )
    {
        ai_vs_ai();

        SDL_PumpEvents();

        keystate = SDL_GetKeyState(NULL);

		// Press 1 for computer vs player
		if (keystate[SDLK_1])
        {
			state = 1;
			SDL_Delay(2000);
			Mix_PauseMusic();
			option_1();
        }

        //Press 2 for player vs player
        if (keystate[SDLK_2])
        {
			state = 2;
			SDL_Delay(2000);
			Mix_PauseMusic();
			option_2();
        }

        //Press 3 for help
        if (keystate[SDLK_3])
        {
            state=0;
            SDL_Delay(2000);
			//Mix_PauseMusic();
            option_3();
            //SDL_BlitSurface(help, NULL, screen, &help_pos);


        }

        //Press 4 for exit
        if (keystate[SDLK_4] || keystate[SDLK_ESCAPE])
        {
            clean_up();
            SDL_Quit();
            exit(0);
        }

    // draw background and paddles
    SDL_BlitSurface(menu, NULL, screen, &rcmenu);
    SDL_BlitSurface(bg, NULL, screen, &rcbg);
    blit_surface();

    SDL_Flip(screen);

    // update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);

  }

}

void after_gameover()
{
    //SDL_FreeSurface(P1score_surface);
    //SDL_FreeSurface(P2score_surface);

    p1_score=0;
    p2_score=0;

    if(state==0)
    {
        select();
        load_files();
        Mix_PlayMusic(sound4,5);
        ResetGame();
    }

    while (state == 0 )
    {
        ai_vs_ai();

        SDL_PumpEvents();

        keystate = SDL_GetKeyState(NULL);

		// Press 1 for computer vs player
		if (keystate[SDLK_1])
        {
			state = 1;
			SDL_Delay(2000);
			Mix_PauseMusic();
			option_1();
        }

        //Press 2 for player vs player
        if (keystate[SDLK_2])
        {
			state = 2;
			SDL_Delay(2000);
			Mix_PauseMusic();
			option_2();
        }

        //Press 3 for help
        if (keystate[SDLK_3])
        {
            state = 0;
            SDL_Delay(2000);
            option_3();
        }

        //Press 4 for exit
        if (keystate[SDLK_4] || keystate[SDLK_ESCAPE])
        {
            clean_up();
            SDL_Quit();
            exit(0);
        }

    // draw background and paddles
    /*
    SDL_BlitSurface(end, &p1, screen, &dest);
    SDL_BlitSurface(end, &p2, screen, &dest);
    SDL_BlitSurface(end, &comp, screen, &dest);
    */


    //SDL_BlitSurface(bg, NULL, screen, &rcbg);
    //SDL_BlitSurface(end, NULL, screen, &dest);
    SDL_BlitSurface(menu, NULL, screen, &rcmenu);
    //SDL_BlitSurface(ball, NULL, screen, &BallRect);
    blit_surface();

    SDL_Flip(screen);

    // update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);

  }
}



void draw_game_over(int p)
{
    //SDL_FreeSurface(P1score_surface);
    //SDL_FreeSurface(P2score_surface);

    SDL_PumpEvents();

    // Grab a snapshot of the keyboard.
    keystate = SDL_GetKeyState(NULL);

    //SDL_Rect comp,p1,p2, dest;

    temp = SDL_LoadBMP("images/gameover1.bmp");
    end = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    comp.x = 0;
    comp.y = 150;
    comp.w = end->w;
    comp.h = 75;

    p1.x=0;
    p1.y=0;
    p1.w=end->w;
    p1.h=75;

    p2.x=0;
    p2.y=80;
    p2.w=end->w;
    p2.h=75;

    dest.x = 500;
    dest.y = 350;
    dest.w = end->w;
    dest.h = end->h;

    switch (p)
    {
        case 1:
            SDL_BlitSurface(end, &p1, screen, &dest);
            break;
        case 2:
            SDL_BlitSurface(end, &p2, screen, &dest);
            break;
        default:
            SDL_BlitSurface(end, &comp, screen, &dest);
    }
        SDL_Flip(screen);
        SDL_Delay(5000);

        state=0;
        //SDL_FreeSurface(P1score_surface);
        //SDL_FreeSurface(P2score_surface);
        //p1_score=0;
        //p2_score=0;

        SDL_Surface *P1score_surface=NULL;
        SDL_Surface *P2score_surface=NULL;

        after_gameover();
}

void option_1()
{
    background();
    load_files();
    ResetGame();

    while (gameover == 0 )
    {
            player_vs_ai();

            SDL_FreeSurface(P2score_surface);
            sprintf(player2HUD,"Score : %d",p2_score);
            P2score_surface=TTF_RenderText_Blended(font,player2HUD, textcolor);

            SDL_FreeSurface(P1score_surface);
            sprintf(player1HUD,"Score : %d",p1_score);
            P1score_surface=TTF_RenderText_Blended(font,player1HUD, textcolor);

            SDL_PumpEvents();

            /* Grab a snapshot of the keyboard. */
            keystate = SDL_GetKeyState(NULL);

            /* Respond to input. */
            if (keystate[SDLK_ESCAPE])
            {
                gameover = 1;
            }
            if(keystate[SDLK_BACKSPACE])
            {
                state=0;
                after_gameover();
            }


    // draw background and paddles
    SDL_BlitSurface(bg, NULL, screen, &rcbg);
    blit_surface();

    SDL_Flip(screen);

    // update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);
  }

}

void option_2()
{
    background();
    load_files();
    ResetGame();

    while (gameover == 0 )
  {
        player_vs_player();

        sprintf(player2HUD,"P2 Score : %d",p2_score);
        P2score_surface=TTF_RenderText_Blended(font,player2HUD, textcolor);

        sprintf(player1HUD,"P1 Score : %d",p1_score);
        P1score_surface=TTF_RenderText_Blended(font,player1HUD, textcolor);


      SDL_PumpEvents();

      // Grab a snapshot of the keyboard
        keystate = SDL_GetKeyState(NULL);

		// Respond to input
		if (keystate[SDLK_ESCAPE])
        {
			gameover = 1;
        }
        if(keystate[SDLK_BACKSPACE])
        {
            state=0;
            after_gameover();
        }

    // draw background and paddles
    SDL_BlitSurface(bg, NULL, screen, &rcbg);
    blit_surface();

    SDL_Flip(screen);

    // update the screen
    SDL_UpdateRect(screen, 0, 0, 0, 0);

  }
}

void option_3()
{
   if(state==0)
    {
        load_files();
        Mix_PlayMusic(sound4,5);
        ResetGame();
    }

    while (state == 0 )
    {
        ai_vs_ai();

        SDL_PumpEvents();

        keystate = SDL_GetKeyState(NULL);

		// Press 1 for computer vs player
		if (keystate[SDLK_BACKSPACE])
        {
			state = 0;
			SDL_Delay(2000);
			Mix_PauseMusic();
			after_gameover();
        }

        //SDL_BlitSurface(menu, NULL, screen, &rcmenu);
        SDL_BlitSurface(help, NULL, screen, &help_pos);
        blit_surface();

        SDL_Flip(screen);

        //update the screen
        SDL_UpdateRect(screen, 0, 0, 0, 0);
    }

}

//main function
int main(int argc, char* args[])
{
    initialize();
    ai_menu();

    //check_score();

   // clean up
    clean_up();

    //exit
    SDL_Delay(1000);
    SDL_Quit();

return 0;
}

