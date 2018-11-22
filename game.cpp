//автор(с): Кудуштеев Алексей
#include "headers.h"
#define M_STATE_PLAY   0
#define M_STATE_CTRL   1
#define M_STATE_HELP   2
#define M_STATE_CLOSE  3
#define M_STATE_MENU   1
#define LIFE_DEFAULT   10
static char g_file_level[] = "level.bin";


quest_game::quest_game(void):base_form(),
                             state_user(U_LEFT_STOP),
                             vel_user(STEP_VELOCITY),
                             stage_pos(0),
                             stage_edge(0),
                             jump_top(0),
                             rc_page(0),
                             move_water(0),
                             time_water(0),
                             time_move(0),
                             dir_water(0),
                             top_water(0),
                             state_game(GAME_MENU),
                             life(0),
                             jump_user(FALSE),
                             fly_user(FALSE),
                             move_right(FALSE),
                             level(0){}

quest_game::~quest_game(){}


//создание
void quest_game::onCreate(HINSTANCE hinst){
	setDelay(5);

	dsound::getDevice(SNDDEV_CREATE, getHandle());

	const CHAR type[] = "WAVE";
	snd_taran.load(hinst, MAKEINTRESOURCEA(IDR_WAV_TARAN), type);
	snd_down.load(hinst, MAKEINTRESOURCEA(IDR_WAV_DOWN), type);
	snd_boom.load(hinst, MAKEINTRESOURCEA(IDR_WAV_BOOM), type);
	snd_lazer.load(hinst, MAKEINTRESOURCEA(IDR_WAV_LAZER), type);
	snd_jump.load(hinst, MAKEINTRESOURCEA(IDR_WAV_JUMP), type);
	snd_head.load(hinst, MAKEINTRESOURCEA(IDR_WAV_HEAD), type);
	snd_music.load_test(hinst, MAKEINTRESOURCEA(IDR_WAV_MUSIC), type);

	lifts.setSize(64);

	menu.initialize(hinst);

	bmp_view.load(hinst, IDB_BMP_VIEW);
	bmp_back.create(getWidth(), getHeight());

	levels.load(hinst, MAKEINTRESOURCEA(IDR_LEVELS), "TEXT");

	bmp_map.load(hinst, IDB_BMP_MAP);
	bmp_map.from_mask(&mask_map);

	tiles2.load(hinst, IDB_BMP_OBJECTS);
	tiles2.from_mask(&mask_t2, RGB(0xFF, 0, 0xFF));	

	tiles.load(hinst, IDB_BMP_BLOCKS);
	bmp_users[0].load(hinst, IDB_BMP_USER);
	bmp_users[1].load(hinst, IDB_BMP_USER1);

	bmp_users[0].from_mask(&mask_users[0]);
	bmp_users[1].from_mask(&mask_users[1]);

	users.setCount(6);
	users[U_LEFT_STOP].create(&bmp_users[0], 3, 3, 200, 0, 0, 1, 3);
	users[U_LEFT_STOP].play(SP_NEXT_PREV_LOOP);

	users[U_LEFT_STEP].create(&bmp_users[0], 3, 3, 150, 1, 0, 2, 3);
	users[U_LEFT_STEP].play(SP_LOOP);

	users[U_LEFT_JUMP].create(&bmp_users[0], 3, 3, 200, 2, 0, 3, 3);

	users[U_RIGHT_STOP].create(&bmp_users[1], 3, 3, 200, 0, 0, 1, 3);
	users[U_RIGHT_STOP].play(SP_NEXT_PREV_LOOP);

	users[U_RIGHT_STEP].create(&bmp_users[1], 3, 3, 150, 1, 0, 2, 3);
	users[U_RIGHT_STEP].play(SP_LOOP);

	users[U_RIGHT_JUMP].create(&bmp_users[1], 3, 3, 200, 2, 0, 3, 3);

	brush_water.create(hinst, IDB_BMP_WATER);
	pen_life.create(PS_SOLID, 3, RGB(0, 0xFF, 0));
	pen_uron.create(PS_SOLID, 3, RGB(0xFF, 0, 0));

	pos_user.x = getWidth()  / 2;
	pos_user.y = getHeight() / 2;
	stage_edge = getWidth() / 3 * 2;
	state_user = U_LEFT_STOP;
	jump_user  = fly_user = FALSE;
	jump_top   = 0;
	vel_user   = STEP_VELOCITY;
	rc_page    = b_width - CELL_SIZE * 4;

	sp_fire.create(40, 40, 100, 0, 0, 1, 5);
	sp_fire.setOffset(0, 68);

	sp_power.create(40, 40, 150, 0, 0, 1, 4);
	sp_power.setOffset(200, 68);

	sp_aliens[0].create(ALIEN_SIZE, ALIEN_SIZE, 110, 0, 0, 1, 4);
	sp_aliens[0].setOffset(0, 108);

	sp_aliens[1].create(ALIEN_SIZE, ALIEN_SIZE, 110, 0, 0, 1, 4);
	sp_aliens[1].setOffset(128, 108);

	sp_aliens[2].create(ALIEN_SIZE, ALIEN_SIZE, 140, 0, 0, 1, 3);
	sp_aliens[2].setOffset(256, 108);

	sp_crash.create(CRASH_CX, CRASH_CY, 140, 0, 0, 1, 5);
	sp_crash.setOffset(0, 140);

	level      = input_file::read_dword(g_file_level);
	life       = LIFE_DEFAULT;
	state_game = GAME_MENU;
}


//ОБРОБКА КЛАВІШ
void quest_game::onKeyPress(LPBYTE keys){
	if(state_game == GAME_PLAY)
		controls(keys);
}


//НАЖАТТЯ КЛАВІШ КЛАВІАТУРИ
void quest_game::onKeyDown(WORD key){
	switch(state_game){
	case GAME_PLAY:
		if(key == VK_ESCAPE){
			setDelay(30);
			snd_music.stop();
			state_game = GAME_PAUSE;
		}
		break;
	case GAME_MENU:
	case GAME_CONTROL:
	case GAME_HELP:
	case GAME_PAUSE:
	case GAME_OVER:
	case GAME_WIN:
		if(key == VK_RETURN)
			exec_menu(menu.getSelected());
		else
			menu.key_down(state_game, key);
		break;
	}
}


//РУХ МИШКИ
void quest_game::onMouseMove(int x, int y){
	switch(state_game){
	case GAME_MENU:
	case GAME_CONTROL:
	case GAME_HELP:
	case GAME_PAUSE:
	case GAME_OVER:
	case GAME_WIN:
		menu.mouse_move(state_game, x, y, b_width);
		break;
	}
}


//КЛАЦАННЯ ЛІВОЇ КНОПКИ
void quest_game::onLeftMouseDown(int x, int y){
	switch(state_game){
	case GAME_MENU:
	case GAME_CONTROL:
	case GAME_HELP:
	case GAME_PAUSE:
	case GAME_OVER:
	case GAME_WIN:
		if(menu.mouse_move(state_game, x, y, b_width))
			exec_menu(menu.getSelected());
		break;
	}	
}


//ОНОВЛЕННЯ АНІМАЦІЇ
void quest_game::onUpdateFrame(DWORD msec){
	if(state_game == GAME_PLAY){
		update(msec);

		if(msec > time_water){
			time_water = msec + 200UL;
			if(dir_water){
				if(--top_water < 1)
					dir_water = 0;
			} else {
				if(++top_water > 11)
					dir_water = 1;
			}
		}

		if(msec > time_move){
			if(move_right && (pos_user.x == stage_edge)){
				move_water -= 2;
				time_move   = msec + 10UL;
			} else {
				--move_water;
				time_move   = msec + 40UL;
			}
			SetBrushOrgEx(getDC(), --move_water, 0, NULL);
		}
	}
}


//ВИВІД НА ЕКРАН - ПРОМАЛЬОВКА
void quest_game::onDrawing(HDC hDC){
	switch(state_game){
	case GAME_PLAY:
		BitBlt(hDC, 0, 0, b_width, b_height, bmp_back.getDC(), 0, 0, SRCCOPY);
		draw_game(hDC);
		draw_life(hDC);
		break;
	case GAME_MENU:
		menu.draw_menu(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map);	
		break;
	case GAME_CONTROL:
		menu.draw_control(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map);	
		break;
	case GAME_HELP:
		menu.draw_help(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map, &tiles2, &mask_t2);	
		break;
	case GAME_LEVEL:
		menu.draw_level(hDC, level, getElapsed(), b_width, b_height);
		if(menu.isTimeout(getElapsed())){
			copy_view();
			activate_level();
			state_game = GAME_PLAY;
		}
		break;
	case GAME_PAUSE:
		menu.draw_pause(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map);	
		break;
	case GAME_OVER:
		menu.draw_over(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map, &bmp_users[0], &mask_users[0]);			
		break;
	case GAME_WIN:
		menu.draw_victory(hDC, getElapsed(), b_width, b_height, &tiles, &bmp_map, &mask_map);			
		break;
	}
}


//ЗНИЩЕННЯ
void quest_game::onDestroy(void){
	state_game = GAME_NOT;
	brush_water.destroy();
	pen_life.destroy();
	pen_uron.destroy();
	menu.destroy();
	bmp_view.destroy();
	bmp_back.destroy();
	aliens.clear();
	circles.clear();
	lazers.clear();
	bmp_map.destroy();
	mask_map.destroy();
	lifts.clear();
	tiles2.destroy();
	mask_t2.destroy();

	users.reset();
	levels.clear();
	for(int i = 0; i < MAX_USERS; ++i){
		bmp_users[i].destroy();
		mask_users[i].destroy();
	}

	snd_taran.destroy();
	snd_down.destroy();
	snd_boom.destroy();
	snd_lazer.destroy();
	snd_jump.destroy();
	snd_head.destroy();
	snd_music.destroy();
	dsound::getDevice(SNDDEV_RELEASE);
}


//------------------------------------------------------------------------------------------------------------------------


//ОНОВЛЕННЯ ГРИ
void quest_game::update(DWORD msec){
	users[state_user].updateFrame(msec);
	flyDown();

	//РУХ ЛІФТІВ
	int         px;
	slift*      pl;
	e_intersect ret;
	for(pslist<slift>::offset p = lifts.getOffset(); p != NULL; ){
		pl = &p->val;
		px = pl->x - stage_pos;

		if(pos_user.x > (px + rc_page)){
			p = lifts.removeAt(p);
			continue;
		} else if(pos_user.x > (px - b_width)){
			if(pl->dir){
				pl->y -= pl->velocity;
				if(pl->y <= pl->pos_start){
					pl->y   = pl->pos_start;
					pl->dir = 0;
				}
			} else {
				pl->y += pl->velocity;
				if(pl->y >= pl->pos_end){
					pl->y   = pl->pos_end;
					pl->dir = 1;
				}
			}

			ret = SquareToSquare(pos_user.x, pos_user.y, CELL_SIZE, px, pl->y, CELL_SIZE);
			if(ret == inter_top)
				jump_user = fly_user = FALSE;
			else if(ret != inter_not){
				jump_user = FALSE;
				fly_user  = TRUE;
			}
		}
		p = p->next;
	}

	//РУХ ВОРОГІВ
	salien* pa;
	DWORD   i;
	for(i = 0; i < aliens.getCount(); ){
		pa = &aliens[i];
		px = pa->x - stage_pos;
		if(pos_user.x > (px + rc_page)){
			aliens.erase(i);
			continue;
		} else if(pos_user.x > (px - b_width)){

			if(pa->dir){
				pa->x -= pa->velocity;
				if(pa->x <= pa->start_x){
					pa->x   = pa->start_x;
					pa->dir = 0;
				}
			} else {
				pa->x += pa->velocity;
				if(pa->x >= pa->end_x){
					pa->x   = pa->end_x;
					pa->dir = 1;
				}
			}

			//ЗІТКНЕННЯ
			if(isSquareToSquare(pos_user.x + 1, pos_user.y + 1, CELL_SIZE - 2, px, pa->y, ALIEN_SIZE)){

				putBoom(pa->x, pa->y, ALIEN_SIZE);
				if(isGameOver())//ВИ ПРОГРАЛИ
					return;
				aliens.erase(i);
				continue;
			}
		}
		++i;
	}

	//ЛАЗЕР
	slazer* lp;
	for(i = 0; i < lazers.getCount(); ){
		lp = &lazers[i];
		px = lp->x - stage_pos;
		if(pos_user.x > (px + rc_page)){
			lazers.erase(i);
			continue;
		} else if(pos_user.x > (px - b_width)){
			if(lp->tick < msec){
				lp->tick   = msec + lp->delay;
				lp->check ^= 1;

				if(lp->check)
					snd_lazer.play();
			}

			if(lp->check == 1){ //ЗВТКНЕННЧ ЛАЗЕРА З ГРАВЦЕМ
				if(isRectToRect(px, lp->y, LAZER_WIDTH, lp->height, pos_user.x, pos_user.y, CELL_SIZE, CELL_SIZE)){
					if(exec_uron())
						return;
				}
			}
		}
		++i;
	}

	//ВИБУХ
	for(i = 0; i < booms.getCount(); ){
		if(booms[i].sp.updateFrame(msec)){
			booms.removeAt(i);
		} else
			++i;
	}

	if(sp_fire.isPlay())
		sp_fire.updateFrame(msec);

	if(sp_power.isPlay())
		sp_power.updateFrame(msec);

	for(i = 0; i < MAX_ALIENS; ++i){
		if(sp_aliens[i].isPlay())
			sp_aliens[i].updateFrame(msec);
	}

	if(sp_crash.isPlay())
		sp_crash.updateFrame(msec);
}


//ВИВІД ГРИ
void quest_game::draw_game(HDC hDC){
	//ВИВІД ТАЙНІВ
	int v;
	int x1 = stage_pos / CELL_SIZE;
	int x2 = min(x1 + PAGE_COLS + 1, MAX_COLS);

	for(int i = 0; i < MAX_ROWS; ++i){
		for(int j = x1; j < x2; ++j){
			v = fmap[i][j];

			if(v == CELL_VERT){
				--v;
				tiles2.draw_clip(hDC, j * CELL_SIZE - stage_pos, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, v * CELL_SIZE, 0, &mask_t2);
			} else if(v == CELL_FIRE)
				sp_fire.draw_clip(hDC, &bmp_map, &mask_map, j * CELL_SIZE - stage_pos, i * CELL_SIZE);
			else if(v == CELL_POWER)
				sp_power.draw_clip(hDC, &bmp_map, &mask_map, j * CELL_SIZE - stage_pos, i * CELL_SIZE);
			else if(v >= CELL_CLIP_OBJ){
				v = v - CELL_CLIP_OBJ + 1;
				tiles2.draw_clip(hDC, j * CELL_SIZE - stage_pos, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, v * CELL_SIZE, 0, &mask_t2);
			} else if(v > CELL_NONE){
				v -= 2;
				tiles.draw(hDC, j * CELL_SIZE - stage_pos, i * CELL_SIZE, CELL_SIZE, CELL_SIZE, v * CELL_SIZE, 0);
			}
		}
	}

	//ВИВІД ЛІФТІВ
	int px;
	for(pslist<slift>::offset p = lifts.getOffset(); p != NULL; p = p->next){
		px = p->val.x - stage_pos;
		if(pos_user.x > (px - b_width))
			tiles2.draw_clip(hDC, px, p->val.y, CELL_SIZE, CELL_SIZE, p->val.type * CELL_SIZE, 0, &mask_t2);
	}

	//ВИВІД ВОРОГІВ
	salien* pa;
	DWORD   k;
	for(k = 0; k < aliens.getCount(); ++k){
		pa = &aliens[k];
		px = pa->x - stage_pos;
		if(pos_user.x > (px - b_width))
			sp_aliens[pa->type].draw_clip(hDC, &bmp_map, &mask_map, px, pa->y);
	}

	//ВИВІД ВЕРТУШОК
	scircle* pc;
	float    deg;
	for(k = 0; k < circles.getCount(); ){
		pc = &circles[k];
		px = pc->x0 - stage_pos;

		if(pos_user.x > (px - b_width)){
			deg    = static_cast<float>(getElapsed()) * pc->velocity;
			pc->x1 = pc->x0 + CELL_SIZE * _tcos(deg);
			pc->y1 = pc->y0 + CELL_SIZE * _tsin(deg);
			bmp_map.draw_rotate(hDC, deg * 100.0f, pc->x1 - stage_pos, pc->y1, ALIEN_SIZE, ALIEN_SIZE, pc->off, 168, &mask_map);

			//ПЕРЕВІРКА НА ЗІТКНЕННЯ
			if(isSquareToCircle(pos_user.x, pos_user.y, CELL_SIZE, pc->x1 + ALIEN_RADIUS - stage_pos, pc->y1 + ALIEN_RADIUS, ALIEN_RADIUS)){

				putBoom(pc->x1 + ALIEN_RADIUS, pc->y1 + ALIEN_RADIUS, ALIEN_RADIUS);
				if(isGameOver())//ВИ ПРОГРАЛИ
					return;
				circles.erase(k);
				continue;
			}
		} else if(pos_user.x > (px + rc_page)){
			circles.erase(k);
			continue;
		}
		++k;
	}

	//ВИВІД ВИБУХУ
	for(DWORD j = 0; j < booms.getCount(); ++j)
		booms[j].sp.draw_clip(hDC, &bmp_map, &mask_map, booms[j].x - stage_pos, booms[j].y);

	int inx = state_user > U_LEFT_JUMP;
	users[state_user].draw_clip(hDC, &bmp_users[inx], &mask_users[inx], pos_user.x, pos_user.y);

	//ВИВІД ЛАЗЕРА
	const slazer* le = lazers.data() + lazers.getCount();
	for(const slazer* lp = lazers.data(); lp != le; ++lp){
		px = lp->x - stage_pos;
		if(pos_user.x > (px - b_width)){
			if(lp->check > 0)
				bmp_map.draw_size(hDC, lp->x - stage_pos, lp->y, LAZER_WIDTH, lp->height, 373, 121, LAZER_WIDTH, 40, SRCPAINT); 
		}
	}

	//ВИБУХ УШКОДЖЕНЬ
	if(sp_crash.isPlay())
		sp_crash.draw_clip(hDC, &bmp_map, &mask_map, pos_user.x + (CELL_SIZE - CRASH_CX)/2, pos_user.y + (CELL_SIZE - CRASH_CY)/2);
}


//УПРАВЛІННЯ ГРОЮ
void quest_game::controls(LPBYTE keys){
	int ret;
	if(!jump_user && !fly_user)
		state_user = (state_user <= U_LEFT_JUMP) ? U_LEFT_STOP : U_RIGHT_STOP;

	move_right = FALSE;

	//РУХ ВПРАВО
	if((keys[VK_RIGHT] & 0x80) || (keys['D'] & 0x80)){	
		move_right = TRUE;
		state_user = (!jump_user && !fly_user) ? U_LEFT_STEP : U_LEFT_JUMP;

		if(pos_user.x < stage_edge){
			pos_user.x += vel_user;
		} else {

			stage_pos += vel_user;
			if((stage_pos + CELL_SIZE) >= STAGE_WIDTH){
				stage_pos = STAGE_WIDTH - CELL_SIZE;

				pos_user.x += vel_user;
				if((pos_user.x + CELL_PSIZE) >= b_width){
					stop_game();

					//РІВЕНЬ ПРОЙДЕНО
					if(++level >= levels.getCount()){ //ВИ ПРОЙШЛИ ВСЮ ГРУ
						level = 0;
						state_game = GAME_WIN;
					} else {
						state_game = GAME_LEVEL;
						menu.setTimeout(getElapsed());
					}
					output_file::write_dword(g_file_level, level);
					return;
				}
			} else
				pos_user.x = stage_edge;
		}

		//ПРОВІРКА НА ЗІТКНЕННЯ З БЛОКОМ
		ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_EDGE);
		if(ret <= CELL_VERT)
			ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE - CELL_EDGE);

		if(ret > CELL_VERT){
			pos_user.x -= vel_user;
			state_user  = U_LEFT_STOP;
		}

	} else if((keys[VK_LEFT] & 0x80) || (keys['A'] & 0x80)){ // движение влево
		state_user = (!jump_user && !fly_user) ? U_RIGHT_STEP : U_RIGHT_JUMP;

		pos_user.x -= vel_user;
		if(pos_user.x < 0){
			pos_user.x = 0;
			state_user = U_RIGHT_STOP;
		}

		//ПРОВІРКА НА ЗІТКНЕННЯ З БЛОКОМ
		ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_EDGE);
		if(ret <= CELL_VERT)
			ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE - CELL_EDGE);

		if(ret > CELL_VERT){
			pos_user.x += vel_user;
			state_user  = U_RIGHT_STOP;
		}

	} else if((keys[VK_UP] & 0x80) || (keys['W'] & 0x80)){ //движение вверх по лестнице
	
		if(isUpDown()){
			state_user  = (state_user <= U_LEFT_JUMP) ? U_LEFT_JUMP : U_RIGHT_JUMP;
			if(users[state_user].isStop())
				users[state_user].play(SP_PLAY);

			pos_user.y -= vel_user;
			ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_EDGE);
			if(ret <= CELL_VERT)
				ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_EDGE);

			if(ret > CELL_VERT){
				pos_user.y += vel_user;
				state_user  = (state_user <= U_LEFT_JUMP) ? U_LEFT_STOP : U_RIGHT_STOP;
			}
		}

	} else if((keys[VK_DOWN] & 0x80) || (keys['S'] & 0x80)){ //РУХ ВНИЗ 

		if(isUpDown()){
			state_user  = (state_user <= U_LEFT_JUMP) ? U_LEFT_JUMP : U_RIGHT_JUMP;
			if(users[state_user].isStop())
				users[state_user].play(SP_PLAYBACK);

			pos_user.y += vel_user;

			ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE - 1);
			if(ret <= CELL_VERT)
				ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE - 1);

			if(ret > CELL_VERT){
				pos_user.y -= vel_user;
				state_user  = (state_user <= U_LEFT_JUMP) ? U_LEFT_STOP : U_RIGHT_STOP;
			}
		}
	}

	//прыжок
	if((keys[VK_SPACE] & 0x80) || (keys[VK_ADD] & 0x80)){

		if(!jump_user && !fly_user && !typeBlock(CELL_MAGNIT)){

			jump_top   = pos_user.y - (CELL_SIZE * 2 + CELL_MID);
			jump_user  = TRUE;
			state_user = (state_user <= U_LEFT_JUMP) ? U_LEFT_JUMP : U_RIGHT_JUMP;

			if(users[state_user].isStop())
				users[state_user].play(SP_PLAY);

			snd_jump.play();
		}
	}
}


//проверка на пересечение с блоком
int quest_game::isBlock(int x, int y, DWORD* pinx){
	int row = y / CELL_SIZE;
	int col = (stage_pos + x) / CELL_SIZE;
	
	if(row < 0)
		row = 0;
	else if(row >= MAX_ROWS)
		row = MAX_ROWS - 1;

	if(col < 0)
		col = 0;
	else if(col >= MAX_COLS)
		col = MAX_COLS - 1;

	if(pinx != NULL)
		*pinx = (static_cast<DWORD>(row) << 16) | static_cast<DWORD>(col);

	return fmap[row][col];
}


//прыжок или полёт вниз, ибо ГРАВИТАЦИЯ
void quest_game::flyDown(void){
	if(isDownFire()){//проверка на огонь или энергию
		if(exec_uron())
			return;
	}

	int   ret;
	DWORD val;
	if(jump_user){ //прыжок

		pos_user.y -= JUMP_VELOCITY;
		ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y, &val);
		if(ret <= CELL_VERT)
			ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y, &val);

		if(ret > CELL_VERT){

			if(ret >= CELL_BOOM_2 && ret <= CELL_BOOM_5){ //если простой блок, то разбить его
				boom_block(static_cast<int>((val >> 16) & 0xFFFF), static_cast<int>(val & 0xFFFF));
				snd_taran.play();
			} else
				snd_head.play();

			pos_user.y += JUMP_VELOCITY;
			jump_user   = FALSE;
			fly_user    = TRUE;
		} else if(pos_user.y < jump_top){
			pos_user.y = jump_top;
			jump_user  = FALSE;
			fly_user   = (ret == CELL_VERT) ? FALSE : TRUE;
		}
		return;
	}

	//полёт вниз, если нет основания
	ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE + 1);
	if(ret == CELL_NONE){

		ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE + 1);
		if(ret == CELL_NONE){
			fly_user    = TRUE;
			pos_user.y += JUMP_VELOCITY;

			ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE + 1);
			if(ret == CELL_NONE)
				ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE + 1);

			if(ret != CELL_NONE){
				pos_user.y = (pos_user.y + CELL_SIZE + 1) / CELL_SIZE * CELL_SIZE - CELL_SIZE;

				if(fly_user)
					snd_down.play();

				fly_user = jump_user = FALSE;
			}
		} else {
			if(fly_user)
				snd_down.play();
			fly_user = jump_user = FALSE;
		}

	} else {
		if(fly_user)
			snd_down.play();
		fly_user = jump_user = FALSE;
	}

	//проверка на полёт в пропасть
	if(pos_user.y > b_height){
		life       = LIFE_DEFAULT;
		state_game = GAME_OVER;
		snd_music.stop();
	}
}


//движение по лестнице
BOOL quest_game::isUpDown(void){
	BOOL res = FALSE;
	int  ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y);
	if(ret != CELL_VERT){

		ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y);
		if(ret != CELL_VERT){

			ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE + 1);
			if(ret != CELL_VERT){

				ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE + 1);
				if(ret == CELL_VERT)
					res = TRUE;

			} else
				res = TRUE;
		} else
			res = TRUE;
	} else
		res = TRUE;
	return res;
}


//тип нижнего блока
bool quest_game::typeBlock(int type){
	int ret1 = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE + 1);
	int ret2 = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE + 1);

	return (((ret1 == type) && (ret2 == type || ret2 == CELL_NONE)) || 
	         (ret2 == type) && (ret1 == CELL_NONE));
}


//разбить блок
void quest_game::boom_block(int row, int col){
	fmap[row][col] = CELL_NONE;

	sboom bm;
	bm.sp.create(76, 68, 100, 0, 0, 1, 5);
	bm.sp.play();
	bm.x = col * CELL_SIZE + (CELL_SIZE - bm.sp.getWidth()) /2;
	bm.y = row * CELL_SIZE + (CELL_SIZE - bm.sp.getHeight())/2;
	booms.add(bm);
}


//проверка на огненный блок
BOOL quest_game::isDownFire(void){
	int ret = isBlock(pos_user.x + CELL_EDGE, pos_user.y + CELL_SIZE + 1);
	if((ret != CELL_FIRE) && (ret != CELL_POWER))
		ret = isBlock(pos_user.x + CELL_SIZE - CELL_EDGE, pos_user.y + CELL_SIZE + 1);
	return ((ret == CELL_FIRE) || (ret == CELL_POWER));
}


//взрыв урона
void quest_game::putBoom(int x, int y, int size){
	snd_boom.play();
	sp_crash.play();
	sboom bm;
	bm.sp.create(76, 68, 100, 0, 0, 1, 5);
	bm.sp.play();
	bm.x = x + (size - bm.sp.getWidth()) /2;
	bm.y = y + (size - bm.sp.getHeight())/2;
	booms.add(bm);
}


//урон пользователю
bool quest_game::exec_uron(void){
	bool ret = false;
	if(sp_crash.isStop()){
		snd_boom.play();
		sp_crash.play();
		if(isGameOver())//вы проиграли
			ret = true;
	}
	return ret;
}


//проверка на проигрыш
bool quest_game::isGameOver(void){
	if(--life < 1){
		life       = LIFE_DEFAULT;
		state_game = GAME_OVER;
		snd_music.stop();
		return true;
	}
	return false;
}


//задний фон
void quest_game::copy_view(void){
	const int size = 150;
	const int left = static_cast<int>(level % 3) * size;

	int mode = SetStretchBltMode(bmp_back.getDC(), HALFTONE);	
	bmp_view.draw_size(bmp_back.getDC(), 0, 0, getWidth(), getHeight(), left, 0, size, 169);
	SetStretchBltMode(bmp_back.getDC(), mode);
}

/*
	1-9 блоки игры
	|   лестница
	#   враг робот-крокодил
	@   враг мехбол
	&   враг электрошок
	A   куст
	B   лазер
	C   деревья
	D   магнит
	E   лифт-1
	F   лифт-2
	G   огонь
	H   электричество
	I   вражеский вращатель-1
	J   вражеский вращатель-2
	X   позиция пользователя   	
*/
//получить уровень
void quest_game::activate_level(void){
	q_level* pl = levels.getAt(level);
	if(pl == NULL)
		return;

	jump_user = fly_user = move_right = FALSE;

	slift  lf;
	salien al;
	lifts.reset();
	aliens.reset();
	circles.reset();
	lazers.reset();

	sp_fire.stop();
	sp_power.stop();
	sp_crash.stop();
	stage_pos  = move_water = top_water = dir_water = 0;
	time_water = time_move  = 0;

	SetBrushOrgEx(getDC(), 0, 0, NULL);

	for(int k = 0; k < MAX_ALIENS; ++k)
		sp_aliens[k].stop();

	int i, c, r;
	for(i = 0; i < MAX_ROWS; ++i)
		memset(fmap[i], 0, MAX_COLS * sizeof(BYTE));

	for(i = 0; i < MAX_ROWS; ++i){
		for(int j = 0; j < MAX_COLS; ++j){
			c = pl->mat[i * MAX_COLS + j];
			if(c == 'X'){
				pos_user.x = static_cast<int>(j * CELL_SIZE);
				pos_user.y = static_cast<int>(i * CELL_SIZE);
				fmap[i][j] = 0;
			} else if(c >= 'A' && c <= 'D'){
				fmap[i][j] = CELL_CLIP_OBJ + static_cast<BYTE>(c - 'A');

				if(c != 'B')
					continue;
				
				for(r = i + 1; r < MAX_ROWS; ++r){
					if(pl->mat[r * MAX_COLS + j] != '0')
						break;
				}

				if(--r <= i)
					continue;

				slazer lz;
				lz.check  = rand() % 2;
				lz.delay  = static_cast<DWORD>(1200 + rand() % 2001);
				lz.x      = j * CELL_SIZE + (CELL_SIZE - LAZER_WIDTH)/2;
				lz.y      = i * CELL_SIZE + CELL_SIZE;
				lz.height = (r - i) * CELL_SIZE;
				lz.tick   = getElapsed() + lz.delay;
				lazers.add(lz);

			} else if(c == 'E' || c == 'F'){

				for(r = i; r < MAX_ROWS; ++r){
					BYTE& b = pl->mat[r * MAX_COLS + j];
					if(b != c)
						break;
					b |= 0x80;
				}

				lf.type      = (c == 'E') ? LIFT_A : LIFT_B;
				lf.velocity  = (c == 'E') ? 2 : 1;
				lf.dir       = static_cast<BYTE>(rand() % 2);
				lf.pos_start = i * CELL_SIZE;
				lf.pos_end   = (r - 1) * CELL_SIZE;
	
				lf.x = j * CELL_SIZE;
				lf.y = (lf.dir) ? lf.pos_end : lf.pos_start;
				lifts.add(lf);

			} else if(c == '#' || c == '@' || c == '&'){ //враг

				for(r = j; r < MAX_COLS; ++r){
					BYTE& b = pl->mat[i * MAX_COLS + r];
					if(b != c)
						break;
					b |= 0x80;
				}

				switch(c){
				case '#':
					al.velocity = 2;
					al.type     = 0;
					break;
				case '@':
					al.velocity = 1;
					al.type     = 1;
					break;
				case '&':
					al.velocity = 1;
					al.type     = 2;
					break;
				}

				al.dir      = static_cast<BYTE>(rand() % 2);
				al.start_x  = j * CELL_SIZE;
				al.end_x    = (r - 1) * CELL_SIZE;

				al.y  = i * CELL_SIZE + CELL_SIZE - ALIEN_SIZE;
				al.x  = (al.dir) ? al.end_x : al.start_x;
				aliens.add(al);

				sp_aliens[al.type].play(SP_NEXT_PREV_LOOP);

			} else if(c >= '1' && c <= '9')
				fmap[i][j] = 1 + static_cast<BYTE>(c - '0');
			else if(c == 'G'){
				fmap[i][j] = CELL_FIRE;
				sp_fire.play(SP_LOOP);
			} else if(c == 'H'){
				fmap[i][j] = CELL_POWER;
				sp_power.play(SP_LOOP);			
			} else if(c == 'I'){
				scircle sc;
				sc.x0  = j * CELL_SIZE + (CELL_SIZE - ALIEN_SIZE)/2;
				sc.y0  = i * CELL_SIZE + (CELL_SIZE - ALIEN_SIZE)/2;
				sc.x1  = sc.y1 = 0;
				sc.off = 348;
				sc.velocity = 0.0018f;
				circles.add(sc);
				fmap[i][j] = 7;
			} else if(c == 'J'){
				scircle sc;
				sc.x0  = j * CELL_SIZE + (CELL_SIZE - ALIEN_SIZE)/2;
				sc.y0  = i * CELL_SIZE + (CELL_SIZE - ALIEN_SIZE)/2;
				sc.x1  = sc.y1 = 0;
				sc.off = 348 - 32;
				sc.velocity = -0.002f;
				circles.add(sc);
				fmap[i][j] = 7;
			} else if(c == '|')
				fmap[i][j] = CELL_VERT;
			else
				fmap[i][j] = CELL_NONE;
		}
	}

	//убрать старший бит
	for(i = 0; i < MAX_ROWS; ++i){
		for(int j = 0; j < MAX_COLS; ++j){
			BYTE& b = pl->mat[i * MAX_COLS + j];
			if(b & 0x80)
				b &= ~0x80;
		}
	}
	snd_music.play(true);
}


//команды меню
void quest_game::exec_menu(int cmd){
	switch(state_game){
	case GAME_MENU:
		switch(cmd){
		case M_STATE_PLAY:
			state_game = GAME_LEVEL;
			life       = LIFE_DEFAULT;
			menu.setTimeout(getElapsed());
			break;
		case M_STATE_CTRL:
			state_game = GAME_CONTROL;
			break;
		case M_STATE_HELP:
			state_game = GAME_HELP;
			break;
		case M_STATE_CLOSE:
			close();
			break;
		default:
			break;
		}
		break;
	case GAME_CONTROL:
	case GAME_HELP:
		state_game = GAME_MENU;
		break;
	case GAME_PAUSE:
		if(cmd == M_STATE_PLAY){
			setDelay(5);
			state_game = GAME_PLAY;
			snd_music.play(true);
		} else if(cmd == M_STATE_MENU){
			setDelay(5);
			state_game = GAME_MENU;
		}
		break;
	case GAME_OVER:
		if(cmd == M_STATE_PLAY){
			state_game = GAME_LEVEL;
			life       = LIFE_DEFAULT;
			menu.setTimeout(getElapsed());
		} else if(cmd == M_STATE_MENU)
			state_game = GAME_MENU;
		break;
	case GAME_WIN:
		if(cmd == M_STATE_PLAY){
			level      = 0;
			state_game = GAME_LEVEL;
			life       = LIFE_DEFAULT;
			menu.setTimeout(getElapsed());
		} else if(cmd == M_STATE_MENU)
			state_game = GAME_MENU;
		break;
	}
	menu.setSelected(0);
}


//вывод индикации жизни
void quest_game::draw_life(HDC hDC){
	HGDIOBJ old = pen_uron.selectObj(hDC);
	gdi_pen::line(hDC, 3, 3, 3 + LIFE_DEFAULT*5, 3);

	pen_life.selectObj(hDC);
	gdi_pen::line(hDC, 3, 3, 3 + life*5, 3);

	SelectObject(hDC, old);

	old = brush_water.selectObj(hDC);

	brush_water.fill(hDC, 0, b_height - 5 - top_water, b_width, 20);

	SelectObject(hDC, old);
}


//остановка звуков игры
void quest_game::stop_game(void){
	snd_taran.stop();
	snd_boom.stop();
	snd_down.stop();
	snd_head.stop();
	snd_jump.stop();
	snd_lazer.stop();
	snd_music.stop();
}
