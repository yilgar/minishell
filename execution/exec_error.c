#include "../minishell.h"

void    handle_exec_error(t_gc *gc, t_env *env, char *cmd, char *err_msg)
{
    write(STDERR_FILENO, "minishell: ", 11);
    write(STDERR_FILENO, cmd, ft_strlen(cmd));
    write(STDERR_FILENO, ": ", 2);
    write(STDERR_FILENO, err_msg, ft_strlen(err_msg));
    write(STDERR_FILENO, "\n", 1);
    env_cleanup_child(env);
    gc_free_all(gc);
}
