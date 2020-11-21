#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

static inline int
compare(int a, int b)
{
    return a - b;
}

typedef enum
{
    RED,
    BLK,
}
color;

typedef struct node
{
    struct node* l;
    struct node* r;
    struct node* p;
    int key;
    color c;
}
node;

typedef struct
{
    node* root;
    int size;
}
tree;

static inline void
tree_delete_1(tree*, node*),
tree_delete_2(tree*, node*),
tree_delete_3(tree*, node*),
tree_delete_4(tree*, node*),
tree_delete_5(tree*, node*),
tree_delete_6(tree*, node*),
tree_insert_1(tree*, node*),
tree_insert_2(tree*, node*),
tree_insert_3(tree*, node*),
tree_insert_4(tree*, node*),
tree_insert_5(tree*, node*);

static inline color
node_color(node* n)
{
    return n ? n->c : BLK;
}

static inline color
node_is_blk(node* n)
{
    return node_color(n) == BLK;
}

static inline color
node_is_red(node* n)
{
    return node_color(n) == RED;
}

static inline void
node_property_1(node* n)
{
    assert(node_is_red(n) || node_is_blk(n));
    if(n)
    {
        node_property_1(n->l);
        node_property_1(n->r);
    }
}

static inline void
node_property_2(node* n)
{
    assert(node_is_blk(n));
}

static inline void
node_property_4(node* n)
{
    if(node_is_red(n))
    {
        assert(node_is_blk(n->l));
        assert(node_is_blk(n->r));
        assert(node_is_blk(n->p));
    }
    if(n)
    {
        node_property_4(n->l);
        node_property_4(n->r);
    }
}

static inline void
node_count_black(node* n, int black_nodes, int* path_black_nodes)
{
    if(node_is_blk(n))
        black_nodes += 1;
    if(n)
    {
        node_count_black(n->l, black_nodes, path_black_nodes);
        node_count_black(n->r, black_nodes, path_black_nodes);
    }
    else
    {
        if(*path_black_nodes == -1)
            *path_black_nodes = black_nodes;
        else
            assert(black_nodes == *path_black_nodes);
    }
}

static inline void
node_property_5(node* root)
{
    int black_count_path = -1;
    node_count_black(root, 0, &black_count_path);
}

static inline void
tree_verify(tree* t)
{
    node_property_1(t->root); // EACH NODE IS EITHER RED OR BLACK.
    node_property_2(t->root); // THE ROOT NODE IS BLACK.
    /* PROPERTY 3: LEAVES ARE COLORED BLACK; IMPLICITLY GUAREENTEED VIA NODE_COLOR. */
    node_property_4(t->root); // EVERY RED NODE HAS TWO BLACK NDOES.
    node_property_5(t->root); // ALL PATHS FROM A NODE HAVE THE SAME NUMBER OF BLACK NODES.
}

static inline node*
node_maximum(node* n)
{
    while(n->r)
        n = n->r;
    return n;
}

static inline node*
node_grandfather(node* n)
{
    return n->p->p;
}

static inline node*
node_sibling(node* n)
{
    if(n == n->p->l)
        return n->p->r;
    else
        return n->p->l;
}

static inline node*
node_uncle(node* n)
{
    return node_sibling(n->p);
}

static inline node*
node_init(int key , color c)
{
    node* result = malloc(sizeof(*result));
    result->key = key;
    result->c = c;
    result->l = result->r = result->p = NULL;
    return result;
}

static inline node*
node_find(tree* t, int key)
{
    node* n = t->root;
    while(n)
    {
        int diff = compare(key, n->key);
        if(diff == 0)
            return n;
        else
        if(diff < 0)
            n = n->l;
        else
            n = n->r;
    }
    return n;
}

static inline void
node_replace(tree* t, node* oldn, node* newn)
{
    if(oldn->p)
    {
        if(oldn == oldn->p->l)
            oldn->p->l = newn;
        else
            oldn->p->r = newn;
    }
    else
        t->root = newn;
    if(newn)
        newn->p = oldn->p;
}

static inline tree*
tree_create(void)
{
    tree* t = malloc(sizeof(*t));
    t->root = NULL;
    t->size = 0;
    return t;
}

static inline void
tree_rotate_l(tree* t, node* n)
{
    node* r = n->r;
    node_replace(t, n, r);
    n->r = r->l;
    if(r->l)
        r->l->p = n;
    r->l = n;
    n->p = r;
}

static inline void
tree_rotate_r(tree* t, node* n)
{
    node* l = n->l;
    node_replace(t, n, l);
    n->l = l->r;
    if(l->r)
        l->r->p = n;
    l->r = n;
    n->p = l;
}

static inline void
tree_insert(tree* t, int key)
{
    node* insert = node_init(key, RED);
    if(t->root)
    {
        node* n = t->root;
        while(1)
        {
            int diff = compare(key, n->key);
            if(diff == 0)
            {
                free(insert);
                return;
            }
            else
            if(diff < 0)
            {
                if(n->l)
                    n = n->l;
                else
                {
                    n->l = insert;
                    break;
                }
            }
            else
            {
                if(n->r)
                    n = n->r;
                else
                {
                    n->r = insert;
                    break;
                }
            }
        }
        insert->p = n;
    }
    else
        t->root = insert;
    tree_insert_1(t, insert);
    t->size += 1;
    tree_verify(t);
}

static inline void
tree_insert_1(tree* t, node* n)
{
    if(n->p)
        tree_insert_2(t, n);
    else
        n->c = BLK;
}

static inline void
tree_insert_2(tree* t, node* n)
{
    if(node_is_blk(n->p))
        return;
    else
        tree_insert_3(t, n);
}

static inline void
tree_insert_3(tree* t, node* n)
{
    if(node_is_red(node_uncle(n)))
    {
        n->p->c = BLK;
        node_uncle(n)->c = BLK;
        node_grandfather(n)->c = RED;
        tree_insert_1(t, node_grandfather(n));
    }
    else
        tree_insert_4(t, n);
}

static inline void
tree_insert_4(tree* t, node* n)
{
    if(n == n->p->r && n->p == node_grandfather(n)->l)
    {
        tree_rotate_l(t, n->p);
        n = n->l;
    }
    else
    if(n == n->p->l && n->p == node_grandfather(n)->r)
    {
        tree_rotate_r(t, n->p);
        n = n->r;
    }
    tree_insert_5(t, n);
}

static inline void
tree_insert_5(tree* t, node* n)
{
    n->p->c = BLK;
    node_grandfather(n)->c = RED;
    if(n == n->p->l && n->p == node_grandfather(n)->l)
        tree_rotate_r(t, node_grandfather(n));
    else
        tree_rotate_l(t, node_grandfather(n));
}

static inline void
tree_delete(tree* t, int key)
{
    node* child;
    node* n = node_find(t, key);
    if(n)
    {
        if(n->l && n->r)
        {
            node* pred = node_maximum(n->l);
            n->key = pred->key;
            n = pred;
        }
        child = n->r ? n->r : n->l;
        if(node_is_blk(n))
        {
            n->c = node_color(child);
            tree_delete_1(t, n);
        }
        node_replace(t, n, child);
        if(n->p == NULL && child)
            child->c = BLK;
        free(n);
        t->size -= 1;
        tree_verify(t);
    }
}

static inline void
tree_delete_1(tree* t, node* n)
{
    if(n->p)
        tree_delete_2(t, n);
}

static inline void
tree_delete_2(tree* t, node* n)
{
    if(node_is_red(node_sibling(n)))
    {
        n->p->c = RED;
        node_sibling(n)->c = BLK;
        if(n == n->p->l)
            tree_rotate_l(t, n->p);
        else
            tree_rotate_r(t, n->p);
    }
    tree_delete_3(t, n);
}

static inline void
tree_delete_3(tree* t, node* n)
{
    if(node_is_blk(n->p)
    && node_is_blk(node_sibling(n))
    && node_is_blk(node_sibling(n)->l)
    && node_is_blk(node_sibling(n)->r))
    {
        node_sibling(n)->c = RED;
        tree_delete_1(t, n->p);
    }
    else
        tree_delete_4(t, n);
}

static inline void
tree_delete_4(tree* t, node* n)
{
    if(node_is_red(n->p)
    && node_is_blk(node_sibling(n))
    && node_is_blk(node_sibling(n)->l)
    && node_is_blk(node_sibling(n)->r))
    {
        node_sibling(n)->c = RED;
        n->p->c = BLK;
    }
    else
        tree_delete_5(t, n);
}

static inline void
tree_delete_5(tree* t, node* n)
{
    if(n == n->p->l
    && node_is_blk(node_sibling(n))
    && node_is_red(node_sibling(n)->l)
    && node_is_blk(node_sibling(n)->r))
    {
        node_sibling(n)->c = RED;
        node_sibling(n)->l->c = BLK;
        tree_rotate_r(t, node_sibling(n));
    }
    else
    if(n == n->p->r
    && node_is_blk(node_sibling(n))
    && node_is_red(node_sibling(n)->r)
    && node_is_blk(node_sibling(n)->l))
    {
        node_sibling(n)->c = RED;
        node_sibling(n)->r->c = BLK;
        tree_rotate_l(t, node_sibling(n));
    }
    tree_delete_6(t, n);
}

static inline void
tree_delete_6(tree* t, node* n)
{
    node_sibling(n)->c = node_color(n->p);
    n->p->c = BLK;
    if(n == n->p->l)
    {
        node_sibling(n)->r->c = BLK;
        tree_rotate_l(t, n->p);
    }
    else
    {
        node_sibling(n)->l->c = BLK;
        tree_rotate_r(t, n->p);
    }
}

static inline void
node_draw(node* self, SDL_Renderer* renderer, int xres, int x, int y, int width)
{
    if(self)
    {
        SDL_Event e;
        SDL_PollEvent(&e);
        if(e.type == SDL_QUIT)
            exit(1);
        int half = width / 2;
        int xa = x - half;
        int xb = x + half;
        int ya = y + 24;
        int red = node_is_red(self);
        SDL_SetRenderDrawColor(renderer, red ? 0xFF : 0x00, red ? 0x00 : 0xFF, 0x00, 0x00);
        SDL_RenderDrawLine(renderer, x, y, xa, ya);
        SDL_RenderDrawLine(renderer, x, y, xb, ya);
        node_draw(self->l,  renderer, xres, xa, ya, half);
        node_draw(self->r, renderer, xres, xb, ya, half);
    }
}

static inline void
render(SDL_Renderer* renderer, int xres, int yres, tree* t)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer);
    node_draw(t->root, renderer, xres / 1, xres / 2, yres / 4, xres / 3);
    SDL_RenderPresent(renderer);
    SDL_Delay(10);
}

int
main(void)
{
    int xres = 1800;
    int yres = 600;
    int capture_delay = 1000;
    int cycles = 1024;
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(xres, yres, 0, &window, &renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(capture_delay);
    tree* t = tree_create();
    for(int i = 0; i < cycles; i++)
    {
        tree_insert(t, rand());
        render(renderer, xres, yres, t);
    }
    while(t->size)
    {
        tree_delete(t, t->root->key);
        render(renderer, xres, yres, t);
    }
    SDL_Delay(capture_delay);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
