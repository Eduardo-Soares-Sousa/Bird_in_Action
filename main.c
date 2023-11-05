#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#define DISP_W 480 //definição de largura da tela do jogo
#define DISP_H 720 //definição de altura da tela do jogo
#define MAX_CANOS 5

typedef struct {
    float posicao_x;
    float posicao_topo_y;
    float posicao_inferior_y;
} Cano;

int main(int argc, char const *argv[]){

    ALLEGRO_DISPLAY *display = NULL;
    ALLEGRO_BITMAP *background = NULL;
    ALLEGRO_BITMAP *ground = NULL;
    ALLEGRO_BITMAP *bird = NULL;
    ALLEGRO_BITMAP *game_over = NULL;
    ALLEGRO_BITMAP *cano_superior = NULL;
    ALLEGRO_BITMAP *cano_inferior = NULL;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;

    bool jogo_comecou = false;

    // Inicialize a Allegro e seus addons necessários
    al_init();
    al_init_image_addon();
    al_install_mouse();

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60); // 60 FPS
    if(!timer) {
        printf("Falha ao criar temporizador\n");
    return -1;
    }

    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_mouse_event_source()); // Registre eventos do mouse
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    ALLEGRO_BITMAP *start_game = al_load_bitmap("images/button_play.png");

    // Crie uma janela
    display = al_create_display(DISP_W, DISP_H);
    al_register_event_source(event_queue, al_get_display_event_source(display));

    // Carregue a imagem de plano de fundo
    background = al_load_bitmap("images/background.bmp");
    ground = al_load_bitmap("images/ground.png");
    bird = al_load_bitmap("images/bird.png");
    game_over = al_load_bitmap("images/gameover.png");
    cano_superior = al_load_bitmap("images/cano_up.png");
    cano_inferior = al_load_bitmap("images/cano_down.png");

    if (!display || !background || !ground || !bird || !game_over || !cano_superior || !cano_inferior) {
        printf("Erro ao inicializar a imagem de plano de fundo\n");
        return -1;
    }
    
    float ground_posicao_x = 0;
    float velocidade_ground = 1;
    float largura_ground = al_get_bitmap_width(ground);

    int pos_birdX = DISP_W/2; 
    int pos_birdY = DISP_H/2;

    double gravidade = 0.2; // Ajuste a intensidade da gravidade conforme necessário
    double velocidade_vertical_passaro = 0.0;

    double largura_passaro = 100; // Nova largura do sprite do pássaro
    double altura_passaro = 100;  // Nova altura do sprite do pássaro

    double largura_botao = 120; // Nova largura do botão
    double altura_botao = 80;  // Nova altura do botão

    double largura_game_over = 250;
    double altura_game_over = 150;

    double posicao_botao_x = DISP_W / 2 - largura_botao / 2; // Posição x do botão
    double posicao_botao_y = DISP_H / 2 - altura_botao / 2 - 100; // Posição y do botão

    // Altura do chão
    int altura_chao = DISP_H - 128;

    bool atualizar = true;
    bool gameover = false;

    Cano canos[MAX_CANOS];

    float largura_cano = al_get_bitmap_width(cano_superior);
    float altura_cano = al_get_bitmap_height(cano_superior);
    float espacamento_entre_cano = 250; // Espaço entre os canos
    float velocidade_cano = 2; // Velocidade dos canos

    for(int i = 0; i < MAX_CANOS; i++){
        canos[i].posicao_x = DISP_W + i * (espacamento_entre_cano + largura_cano);
        canos[i].posicao_topo_y = rand() % (DISP_H - 300) + 100; // Posição aleatória para o cano superior
        canos[i].posicao_inferior_y = canos[i].posicao_topo_y + espacamento_entre_cano;
    }

    // Loop principal
    bool jogo = true;
    while(jogo){

        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            jogo = false;
        }else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            if(!jogo_comecou && !gameover){
                // Verifique se o mouse foi clicado dentro da área do botão de iniciar
                if (event.mouse.x >= posicao_botao_x && event.mouse.x <= posicao_botao_x + largura_botao &&
                    event.mouse.y >= posicao_botao_y && event.mouse.y <= posicao_botao_y + altura_botao) {
                    jogo_comecou = true;
                }
            }else if(gameover){
                // Reinicie o jogo
                gameover = false;
                pos_birdX = DISP_W / 2;
                pos_birdY = DISP_H / 2;
                velocidade_vertical_passaro = 0.0;
                jogo_comecou = true;

                // Reinicie a posição dos canos
                for(int i = 0; i < MAX_CANOS; i++){
                    canos[i].posicao_x = DISP_W + i * (espacamento_entre_cano + largura_cano);
                    canos[i].posicao_topo_y = rand() % (DISP_H - 300) + 100;
                    canos[i].posicao_inferior_y = canos[i].posicao_topo_y + espacamento_entre_cano;
                }
            }else if(jogo_comecou){
                // O jogador clicou no mouse, faça o pássaro subir
                velocidade_vertical_passaro = -4.0; // Valor negativo para subir
            }
        }else if(event.type == ALLEGRO_EVENT_TIMER){
            atualizar = true;
        }

        if(atualizar && al_is_event_queue_empty(event_queue)){
            atualizar = false;

            al_clear_to_color(al_map_rgb(0, 0, 0)); // Limpe a tela com uma cor de fundo

            // Desenhe a imagem de plano de fundo
            al_draw_bitmap(background, 0, 0, 0);

            if(jogo_comecou && !gameover){
                // Atualize a posição vertical do pássaro
                pos_birdY += velocidade_vertical_passaro;

                // Aplique a gravidade (aumente a velocidade vertical do pássaro)
                velocidade_vertical_passaro += gravidade;


                for(int i = 0; i < MAX_CANOS; i++){
                    canos[i].posicao_x -= velocidade_cano;

                    // Quando um cano sai da tela à esquerda, reposicione-o à direita
                    if(canos[i].posicao_x + largura_cano < 0){
                        canos[i].posicao_x = DISP_W;
                        canos[i].posicao_topo_y = rand() % (DISP_H - 100) + 300; // Posição aleatória para o cano superior
                        canos[i].posicao_inferior_y = canos[i].posicao_x + espacamento_entre_cano;
                    }

                    // Verifique a colisão com o pássaro
                    if((pos_birdX + largura_passaro / 2 > canos[i].posicao_x) && (pos_birdX - largura_passaro / 2 < canos[i].posicao_x + largura_cano) && (pos_birdY - altura_passaro / 2 < canos[i].posicao_topo_y) || (pos_birdY + altura_passaro / 2 > canos[i].posicao_inferior_y)) {
                        gameover = true;
                    }

	                al_draw_bitmap(cano_superior, canos[i].posicao_x, canos[i].posicao_topo_y - altura_cano, 0);
                    al_draw_bitmap(cano_inferior, canos[i].posicao_x, canos[i].posicao_inferior_y, 0);
                }

                // Verifique a colisão com o chão
                if(pos_birdY + altura_passaro / 2 - 50 > altura_chao){
                    gameover = true; // Indica que o jogo acabou
                }


                ground_posicao_x -= velocidade_ground;

                if(ground_posicao_x <= - largura_ground){
                    ground_posicao_x = 0;
                }

                al_draw_scaled_bitmap(bird, 0, 0, al_get_bitmap_width(bird), al_get_bitmap_height(bird), pos_birdX - largura_passaro / 2, pos_birdY - altura_passaro / 2, largura_passaro, altura_passaro, 0);
            }else if (gameover){
                // Exiba a imagem "game_over"
                al_draw_scaled_bitmap(game_over, 0, 0, al_get_bitmap_width(game_over), al_get_bitmap_height(game_over), posicao_botao_x - 70, posicao_botao_y, largura_game_over, altura_game_over, 0);
            }else{
                al_draw_scaled_bitmap(start_game, 0, 0, al_get_bitmap_width(start_game), al_get_bitmap_height(start_game), posicao_botao_x, posicao_botao_y, largura_botao, altura_botao, 0);
            }

            // Desenhe várias instâncias da imagem de chão para criar a repetição
            for(float x = ground_posicao_x; x < al_get_display_width(display); x += largura_ground){
                al_draw_bitmap(ground, x, altura_chao, 0); // Desenhe o chão repetido horizontalmente
            }

            al_flip_display(); // Atualize a tela
        }
    }

    // Libere os recursos
    al_destroy_bitmap(background);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}

/*
if (!game_started) {
    al_draw_scaled_bitmap(start_game, 0, 0, al_get_bitmap_width(start_game), al_get_bitmap_height(start_game), DISP_W / 2 - largura_botao / 2, DISP_H / 2 - altura_botao / 2, largura_botao, altura_botao, 0);
}
al_draw_scaled_bitmap(bird, 0, 0, al_get_bitmap_width(bird), al_get_bitmap_height(bird), pos_birdX - largura_passaro / 2, pos_birdY - altura_passaro / 2, largura_passaro, altura_passaro, 0);
*/




/*
for (int i = 0; i < MAX_CANOS; i++) {
    canos[i].x -= cano_speed;

    // Quando um cano sai da tela à esquerda, reposicione-o à direita
    if (canos[i].x + cano_width < 0) {
        canos[i].x = DISP_W;
        canos[i].y_top = rand() % (DISP_H - 300) + 100;
        canos[i].y_bottom = canos[i].y_top + espacamento_entre_cano;
    }
                
    // Verifique a colisão com o pássaro
    if (pos_birdX + largura_passaro / 2 > canos[i].x &&
        pos_birdX - largura_passaro / 2 < canos[i].x + cano_width &&
        (pos_birdY - altura_passaro / 2 < canos[i].y_top || pos_birdY + altura_passaro / 2 > canos[i].y_bottom)) {
        gameover = true;
    }

    // Verifique se o pássaro passou com segurança entre os canos
                    if (pos_birdX > canos[0].x + cano_width / 2 && pos_birdX < canos[0].x + cano_width) {
                        // Você pode adicionar aqui uma lógica para aumentar a pontuação do jogador
                    }
}
*/