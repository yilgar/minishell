#include "../minishell.h"

int	builtin_pwd(t_gc *gc, char **args)
{
	char	*cwd;

	(void)args;
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("pwd");
		return (1);
	}
	gc_track(gc, cwd);
	printf("%s\n", cwd);
	return (0);
}
