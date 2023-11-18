#include "TAD_Pilha/Pilha.h"

#define DISP_W 480 //largura da tela do jogo
#define DISP_H 720 //altura da tela do jogo
#define MAX_CANOS 5

typedef struct {
    float posicao_x;
    float posicao_topo_y;
    float posicao_inferior_y;
} Cano;

void gerarCano(Cano *cano);
void inciar_Allegro();

int main(int argc, char const *argv[]){

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_BITMAP *background = NULL;
    ALLEGRO_BITMAP *ground = NULL;
    ALLEGRO_BITMAP *bird = NULL;
    ALLEGRO_BITMAP *game_over = NULL;
    ALLEGRO_BITMAP *cano_superior = NULL;
    ALLEGRO_BITMAP *cano_inferior = NULL;
    ALLEGRO_EVENT_QUEUE *fila = NULL;

    Cano canos[MAX_CANOS];
    Pilha *pilha = iniciar_pilha();  

    inciar_Allegro();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60);
    if(!timer) {
        printf("Falha ao criar temporizador\n");
    return -1;
    }

    fila = al_create_event_queue();
    al_register_event_source(fila, al_get_mouse_event_source());
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_start_timer(timer);

    ALLEGRO_BITMAP *start_game = al_load_bitmap("images/button_play.png");

    //Criar janela
    display = al_create_display(DISP_W, DISP_H);
    al_register_event_source(fila, al_get_display_event_source(display));

    //Carregar imagens
    background = al_load_bitmap("images/background.bmp");
    ground = al_load_bitmap("images/ground.png");
    bird = al_load_bitmap("images/bird.png");
    game_over = al_load_bitmap("images/gameover.png");
    cano_superior = al_load_bitmap("images/cano_up.png");
    cano_inferior = al_load_bitmap("images/cano_down.png");

    if (!display || !background || !ground || !bird || !game_over || !cano_superior || !cano_inferior) {
        printf("Erro ao inicializar as imagens. Tente novamente\n");
        return -1;
    }
    
    float ground_posicao_x = 0;
    float velocidade_ground = 1;
    float largura_ground = al_get_bitmap_width(ground);

    int pos_birdX = DISP_W/2; 
    int pos_birdY = DISP_H/2;

    double gravidade = 0.2;
    double queda_passaro = 0.0;

    double largura_passaro = 100;
    double altura_passaro = 100;

    double largura_botao = 120;
    double altura_botao = 80;

    double largura_game_over = 250;
    double altura_game_over = 150;

    double posicao_botao_x = DISP_W / 2 - largura_botao / 2;
    double posicao_botao_y = DISP_H / 2 - altura_botao / 2 - 100;

    int altura_chao = DISP_H - 128;

    bool atualizar = true;
    bool gameover = false;
    bool jogo_comecou = false;

    float largura_cano = al_get_bitmap_width(cano_superior);
    float altura_cano = al_get_bitmap_height(cano_superior);
    float espacamento_entre_cano = 250;
    float velocidade_cano = 2;

    for (int i = 0; i < MAX_CANOS; i++) {
        gerarCano(&canos[i]);
    }

    int pontuacao = 0;
    bool passou = false;
    bool jogo = true;
    while(jogo){

        ALLEGRO_EVENT event;
        al_wait_for_event(fila, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo = false;
        }else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            if(!jogo_comecou && !gameover){
                //Verificar se o botão iniciar foi acionado
                if (event.mouse.x >= posicao_botao_x && event.mouse.x <= posicao_botao_x + largura_botao &&
                    event.mouse.y >= posicao_botao_y && event.mouse.y <= posicao_botao_y + altura_botao) {
                    jogo_comecou = true;
                }
            }else if(gameover){
                gameover = false;
                pos_birdX = DISP_W / 2;
                pos_birdY = DISP_H / 2;
                queda_passaro = 0.0;
                jogo_comecou = true;

                for(int i = 0; i < MAX_CANOS; i++){
                    canos[i].posicao_x = DISP_W + i * (espacamento_entre_cano + largura_cano);
                    canos[i].posicao_topo_y = rand() % (DISP_H - 300) + 100;
                    canos[i].posicao_inferior_y = canos[i].posicao_topo_y + espacamento_entre_cano;
                }
            }else if(jogo_comecou){
                queda_passaro = -4.0;

                empilhar(pos_birdY, pilha);
            }
        }else if(event.type == ALLEGRO_EVENT_TIMER){
            atualizar = true;
        }

        if(atualizar && al_is_event_queue_empty(fila)){
            atualizar = false;

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_bitmap(background, 0, 0, 0);

            if(jogo_comecou && !gameover){
                pos_birdY += queda_passaro;

                queda_passaro += gravidade;

                if (!esta_vazia(pilha) && pos_birdY >= desempilhar(pilha)) {
                    pos_birdY = desempilhar(pilha);
                }

                if(pos_birdX > canos[0].posicao_x + al_get_bitmap_width(cano_superior) / 2 && pos_birdX < canos[0].posicao_x + al_get_bitmap_width(cano_superior)){
                    if(!passou){
                        pontuacao++;
                        printf("Pontuação: %d\n", pontuacao);
                        passou = true;
                    }
                } else {
                    passou = false;
                }

                ALLEGRO_FONT *fonte = al_create_builtin_font();
                al_draw_textf(fonte, al_map_rgb(255, 255, 255), 10, 10, 0, "Pontuacao: %d", pontuacao);
                al_destroy_font(fonte);

                for(int i = 0; i < MAX_CANOS; i++){
                    canos[i].posicao_x -= velocidade_cano;
                    if(canos[i].posicao_x + largura_cano < 0){
                        gerarCano(&canos[i]);
                    }

                    if((pos_birdX + largura_passaro / 2 > canos[i].posicao_x) && (pos_birdX - largura_passaro / 2 < canos[i].posicao_x + largura_cano) && ((pos_birdY - altura_passaro / 2 < canos[i].posicao_topo_y) || (pos_birdY + altura_passaro / 2 > canos[i].posicao_inferior_y))) {
                        gameover = true;
                    }

	                al_draw_bitmap(cano_superior, canos[i].posicao_x, canos[i].posicao_topo_y - altura_cano, 0);
                    al_draw_bitmap(cano_inferior, canos[i].posicao_x, canos[i].posicao_inferior_y, 0);
                }

                if(pos_birdY + altura_passaro / 2 - 50 > altura_chao){
                    gameover = true;
                }


                ground_posicao_x -= velocidade_ground;

                if(ground_posicao_x <= - largura_ground){
                    ground_posicao_x = 0;
                }

                al_draw_scaled_bitmap(bird, 0, 0, al_get_bitmap_width(bird), al_get_bitmap_height(bird), pos_birdX - largura_passaro / 2, pos_birdY - altura_passaro / 2, largura_passaro, altura_passaro, 0);
            }else if (gameover){
                //Exiba a imagem "game_over"
                al_draw_scaled_bitmap(game_over, 0, 0, al_get_bitmap_width(game_over), al_get_bitmap_height(game_over), posicao_botao_x - 70, posicao_botao_y, largura_game_over, altura_game_over, 0);
                pontuacao = 0;
            }else{
                al_draw_scaled_bitmap(start_game, 0, 0, al_get_bitmap_width(start_game), al_get_bitmap_height(start_game), posicao_botao_x, posicao_botao_y, largura_botao, altura_botao, 0);
            }

            for(float x = ground_posicao_x; x < al_get_display_width(display); x += largura_ground){
                al_draw_bitmap(ground, x, altura_chao, 0);
            }

            al_flip_display();
        }
    }

    al_destroy_bitmap(background);
    al_destroy_bitmap(ground);
    al_destroy_bitmap(bird);
    al_destroy_bitmap(game_over);
    al_destroy_bitmap(cano_superior);
    al_destroy_bitmap(cano_inferior);
    al_destroy_display(display);
    al_destroy_event_queue(fila);

    return 0;
}

void gerarCano(Cano *cano) {
    int espacamento_entre_cano = 250;
    cano->posicao_x = DISP_W;
    cano->posicao_topo_y = rand() % (DISP_H - 300) + 100;
    cano->posicao_inferior_y = cano->posicao_topo_y + espacamento_entre_cano;
}

void inciar_Allegro(){

    al_init();
    al_init_image_addon();
    al_install_mouse();
}