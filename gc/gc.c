#include "../minishell.h"

void    gc_init(t_gc *gc)
{
    gc->head = NULL;
}

void    *gc_track(t_gc *gc, void *ptr)
{
    t_gc_node *node;

    if (!ptr)
        return (NULL);
    node = malloc(sizeof(t_gc_node));
    if (!node)
        return (NULL);
    node->ptr = ptr;
    node->next = gc->head;
    gc->head = node;
    return (ptr);
}

void **gc_track_array(t_gc *gc, void **array)
{
    int     i;
    if (!array)
        return (NULL);
    i = -1;
    while(array[++i])
        gc_track(gc, array[i]);
    return (array);
}

void    gc_free_all(t_gc *gc)
{
    t_gc_node *cur = gc->head;
    t_gc_node *tmp;

    while (cur)
    {
        tmp = cur->next;
        free(cur->ptr);
        free(cur); 
        cur = tmp;
    }
    gc->head = NULL;
}