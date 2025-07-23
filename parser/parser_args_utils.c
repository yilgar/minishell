#include "../minishell.h"

void	compact_args_array(char **args, int count)
{
	int	j;
	int	k;

	j = 0;
	k = 0;
	while (k < count)
	{
		if (args[k] && args[k][0] != '\0')
			args[j++] = args[k];
		k++;
	}
	args[j] = NULL;
}
