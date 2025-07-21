#include "../minishell.h"

static int	check_pipe_syntax(t_token *current)
{
	if (current->type == TOKEN_PIPE && current->next
		&& current->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
		return (0);
	}
	return (1);
}

static int	check_redirection_syntax_simple(t_token *current)
{
	if (!current->next || current->next->type == TOKEN_EOF)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `newline'\n", STDERR_FILENO);
		return (0);
	}
	if (is_redirection_token(current->next)
		|| current->next->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", STDERR_FILENO);
		ft_putstr_fd(current->next->value, STDERR_FILENO);
		ft_putstr_fd("'\n", STDERR_FILENO);
		return (0);
	}
	return (1);
}

int	validate_syntax(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	if (current && current->type == TOKEN_PIPE)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `|'\n", STDERR_FILENO);
		return (0);
	}
	while (current && current->type != TOKEN_EOF)
	{
		if (is_redirection_token(current))
		{
			if (!check_redirection_syntax_simple(current))
				return (0);
		}
		if (!check_pipe_syntax(current))
			return (0);
		current = current->next;
	}
	return (1);
}
