#include "../minishell.h"

static int	process_pipe_token(t_token **tokens, int *incomplete_pipe)
{
	*tokens = (*tokens)->next;
	if (!*tokens || (*tokens)->type == TOKEN_EOF)
	{
		*incomplete_pipe = 1;
		return (1);
	}
	if ((*tokens)->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		return (-1);
	}
	return (0);
}

static t_pipeline	*init_pipeline(t_gc *gc, t_token *tokens,
		int *incomplete_pipe)
{
	t_pipeline	*pipeline;

	*incomplete_pipe = 0;
	if (!validate_syntax(tokens))
		return (NULL);
	if (tokens && tokens->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n",
			STDERR_FILENO);
		return (NULL);
	}
	pipeline = gc_track(gc, malloc(sizeof(t_pipeline)));
	if (!pipeline)
		return (NULL);
	pipeline->commands = NULL;
	pipeline->exit_status = 0;
	return (pipeline);
}

static void	add_command_to_pipeline(t_pipeline *pipeline, t_cmd *cmd,
		t_cmd **last_cmd)
{
	if (!pipeline->commands)
		pipeline->commands = cmd;
	else
		(*last_cmd)->next = cmd;
	*last_cmd = cmd;
}

static t_pipeline	*process_commands_loop(t_gc *gc, t_token *tokens,
		t_pipeline *pipeline, int *incomplete_pipe)
{
	t_cmd	*cmd;
	t_cmd	*last_cmd;
	int		pipe_result;

	last_cmd = NULL;
	while (tokens && tokens->type != TOKEN_EOF)
	{
		cmd = parse_command(gc, &tokens);
		if (!cmd)
			break ;
		add_command_to_pipeline(pipeline, cmd, &last_cmd);
		if (tokens && tokens->type == TOKEN_PIPE)
		{
			pipe_result = process_pipe_token(&tokens, incomplete_pipe);
			if (pipe_result == 1)
				return (pipeline);
			if (pipe_result == -1)
				return (NULL);
		}
		else
			break ;
	}
	return (pipeline);
}

t_pipeline	*parse_pipeline(t_gc *gc, t_token *tokens, int *incomplete_pipe)
{
	t_pipeline	*pipeline;

	pipeline = init_pipeline(gc, tokens, incomplete_pipe);
	if (!pipeline)
	{
		gc_free_all(gc);
		return (NULL);
	}
	return (process_commands_loop(gc, tokens, pipeline, incomplete_pipe));
}
