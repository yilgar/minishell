#include "../minishell.h"

int	builtin_echo(t_gc *gc, char **args)
{
	int	i;
	int	newline;

	(void)gc;
	i = 1;
	newline = 1;
	
	while (args[i] && ft_strncmp(args[i], "-n", 2) == 0 && args[i][2] == '\0')
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}
