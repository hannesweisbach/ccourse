#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <limits.h>
#include <assert.h>

#define FIELD_LENGTH 3

const unsigned char NO_PLAYER = ' ';

struct game {
  unsigned char field[FIELD_LENGTH * FIELD_LENGTH];
};

struct player {
  unsigned char symbol;
  int (*move_function)(const struct game * g, const unsigned char player);
};

void output(const struct game *g) {
  for (ssize_t j = 0; j < FIELD_LENGTH; j++) {
    for (ssize_t i = 0; i < FIELD_LENGTH; i++) {
      printf("%c%c", g->field[j * FIELD_LENGTH + i],
             (i < (FIELD_LENGTH - 1)) ? '|' : '\n');
    }
    if (j < (FIELD_LENGTH - 1))
      printf("-+-+-\n");
  }
}

void make_clear(struct game *g) {
  for (size_t i = 0; i < sizeof(g->field); i++)
    g->field[i] = NO_PLAYER;
}

_Bool valid_move(const struct game * g, size_t field) {
  return ((field < sizeof(g->field)) && (g->field[field] == NO_PLAYER));
}

unsigned char winner(const struct game *g) {
  /* case 1: --- */
  for (size_t j = 0; j < sizeof(g->field); j += FIELD_LENGTH)
    if (g->field[j] != NO_PLAYER && g->field[j] == g->field[j + 1] &&
        g->field[j] == g->field[j + 2])
      return g->field[j];

  /* case 2: ||| */
  for (ssize_t j = 0; j < FIELD_LENGTH; j++)
    if (g->field[j] != NO_PLAYER &&
        g->field[j] == g->field[FIELD_LENGTH * 1 + j] &&
        g->field[j] == g->field[FIELD_LENGTH * 2 + j])
      return g->field[j];

  /* case 3: /\ */
  if (g->field[0] == g->field[4] && g->field[0] == g->field[8])
    return g->field[0];
  if (g->field[2] == g->field[4] && g->field[2] == g->field[6])
    return g->field[2];

  return NO_PLAYER;
}

_Bool game_is_on(const struct game * g) {
  /* no winner yet */
  if(winner(g) != NO_PLAYER)
    return 0;

  /* still space on board */
  for(size_t i = 0; i < sizeof(g->field); i++) {
    if(g->field[i] == NO_PLAYER)
      return 1;
  }

  return 0;
}

void do_move(const unsigned char player, const size_t move,
             struct game *g) {
  g->field[move] = player;
}

unsigned char other_player(unsigned char player) {
  assert(player == 'x' || player == 'o');
#if 0
  if(player == 'x')
    return 'o';
  else
    return 'x';
#else
  return (player == 'x') ? 'o' : 'x';
#endif
}

int evaluate(const struct game *game, const char player) {
  if (winner(game) == player) {
    return 1;
  } else if (winner(game) == ' ') {
    return 0;
  } else {
    return -1;
  }
}

int miniMax(const struct game *game, const char player, int depth,
            size_t *best_move) {
  if (!game_is_on(game))
    return evaluate(game, player);
  int maxWert = INT_MIN;
  for (size_t move = 0; move < sizeof(game->field); move++) {
    if (!valid_move(game, move))
      continue;

    struct game copy_ = *game;
    do_move(player, move, &copy_);
    int wert =
        -miniMax(&copy_, (player == 'x') ? 'o' : 'x', depth + 1, best_move);
    if (wert > maxWert) {
      maxWert = wert;
      if (depth == 0)
        *best_move = move;
    }
  }
  return maxWert;
}

int get_move(const struct game * g, const unsigned char player) {
  output(g);
  printf("Zug für Spieler '%c'?: ", player);
  fflush(stdout);
  int move = -1;
  scanf("%d", &move);
  return move;
}


int get_smart_computer_move(const struct game * g, const unsigned char player) {
  size_t best_move = 0;
  miniMax(g, player, 0, &best_move);
  return best_move;
}


int get_computer_move(const struct game * g, const unsigned char player) {
  (void)(player);
  for(size_t move = 0; move < sizeof(g->field); move++)
    if(valid_move(g, move))
      return move;

  return 0;
}

int main() {
  struct game g = {
    .field = { 'x', 'o', ' ', 'j', 'k', 'l', 'x', 'x', 'x' }};
  make_clear(&g);

#if 1
  struct player players[] = { { 'x', &get_move }, { 'o', &get_smart_computer_move } };
#else
  struct player players[] = { { 'x', &get_smart_computer_move },
                              { 'o', &get_smart_computer_move } };
#endif
  size_t player_idx = 0;
  while(game_is_on(&g)) {
    struct player * player = &players[player_idx & 1];
    size_t move = player->move_function(&g, player->symbol);
    if(valid_move(&g, move)) {
      do_move(player->symbol, move, &g);
    } else {
      printf("Move %zd of player %c is invalid. Game lost.\n", move, player->symbol);
      return -1;
    }
    if(winner(&g) != NO_PLAYER) {
      printf("Player %c has won.\n", player->symbol);
      output(&g);
      return 0;
    }
    player_idx++;
  }
  printf("Tie.\n");
  output(&g);
}
