/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yilgar <yilgar@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/22 13:23:14 by yilgar            #+#    #+#             */
/*   Updated: 2025/06/22 13:23:15 by yilgar           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

t_env	*env_init_copy(t_gc *gc, char **envp)
{
	t_env	*env;
	int		i;
	int		len;

	env = malloc(sizeof(t_env));
	len = env_len(envp);
	(void)gc;
	env->original_envp = envp;
	env->envp = malloc(sizeof(char *) * (len + 1));
	i = -1;
	while (++i < len)
		env->envp[i] = ft_strdup(envp[i]);
	env->envp[len] = NULL;
	return (env);
}

void	env_cleanup(t_env *env)
{
	int	i;

	if (!env)
		return ;
	if (env->envp)
	{
		i = 0;
		while (env->envp[i])
		{
			free(env->envp[i]);
			i++;
		}
		free(env->envp);
	}
	free(env);
}

void	env_cleanup_child(t_env *env)
{
	int	i;

	if (!env)
		return ;
	if (env->envp)
	{
		i = 0;
		while (env->envp[i])
		{
			free(env->envp[i]);
			i++;
		}
		free(env->envp);
	}
	free(env);
}
