#include "../minishell.h"

static int	is_valid_numeric_arg(char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

int	builtin_exit(t_gc *gc, t_env *env, char **args)
{
	int	exit_code;

	printf("exit\n");
	if (args[1])
	{
		if (args[2])
		{
			handle_exec_error(gc, env, "exit", "too many arguments");
			exit(1);
		}
		if (!is_valid_numeric_arg(args[1]))
		{
			handle_exec_error(gc, env, args[1], "numeric argument required");
			exit(2);
		}
		exit_code = ft_atoi(args[1]);
		exit_code = exit_code & 255;
	}
	else
		exit_code = 0;
	env_cleanup(env);
	gc_free_all(gc);
	exit(exit_code);
}

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strncmp(cmd, "echo", 4) == 0 && cmd[4] == '\0')
		return (1);
	if (ft_strncmp(cmd, "cd", 2) == 0 && cmd[2] == '\0')
		return (1);
	if (ft_strncmp(cmd, "pwd", 3) == 0 && cmd[3] == '\0')
		return (1);
	if (ft_strncmp(cmd, "export", 6) == 0 && cmd[6] == '\0')
		return (1);
	if (ft_strncmp(cmd, "unset", 5) == 0 && cmd[5] == '\0')
		return (1);
	if (ft_strncmp(cmd, "env", 3) == 0 && cmd[3] == '\0')
		return (1);
	if (ft_strncmp(cmd, "exit", 4) == 0 && cmd[4] == '\0')
		return (1);
	return (0);
}

int	execute_builtin(t_gc *gc, t_env *env, char **args)
{
	if (!args || !args[0])
		return (1);
	if (ft_strncmp(args[0], "echo", 4) == 0 && args[0][4] == '\0')
		return (builtin_echo(gc, args));
	if (ft_strncmp(args[0], "cd", 2) == 0 && args[0][2] == '\0')
		return (builtin_cd(gc, env, args));
	if (ft_strncmp(args[0], "pwd", 3) == 0 && args[0][3] == '\0')
		return (builtin_pwd(gc, args));
	if (ft_strncmp(args[0], "export", 6) == 0 && args[0][6] == '\0')
		return (builtin_export(gc, env, args));
	if (ft_strncmp(args[0], "unset", 5) == 0 && args[0][5] == '\0')
		return (builtin_unset(gc, env, args));
	if (ft_strncmp(args[0], "env", 3) == 0 && args[0][3] == '\0')
		return (builtin_env(gc, env, args));
	if (ft_strncmp(args[0], "exit", 4) == 0 && args[0][4] == '\0')
		return (builtin_exit(gc, env, args));
	return (1);
}
