#include "../minishell.h"

int	env_len(char **envp)
{
	int	i = 0;
	while (envp && envp[i])
		i++;
	return (i);
}
