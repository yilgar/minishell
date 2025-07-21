#include "../minishell.h"

			// write(STDERR_FILENO, "minishell: ", 11);
			// write(STDERR_FILENO, cmd_name, ft_strlen(cmd_name));
			// write(STDERR_FILENO, ": Permission denied\n", 20);
			// env_cleanup_child(env);
			// gc_free_all(gc);
			// exit(126);

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


// export PATH=$PATH:/home/codespaces çalışmıyor
// export PATH="$PATH:/home/codespaces" çalışıyor


// minishell> echo "/desktop/$HOME/desktop"
// /desktop//home/codespace/desktop
// minishell> echo /desktop$HOME/desktop
// /desktop

//heredoc tırnaklıysa  pure string olarak alacak
//heredoc tırnaksızsa  $HOME gibi değişkenleri çözecek