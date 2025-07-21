#include "../minishell.h"

t_token	*create_token(t_gc *gc, t_token_type type, char *value)
{
	t_token	*token;

	token = gc_track(gc, malloc(sizeof(t_token)));
	if (!token)
		return (NULL);
	token->type = type;
	if (value)
		token->value = gc_track(gc, ft_strdup(value));
	else
		token->value = NULL;
	token->has_space_before = 0;
	token->from_env_expansion = 0;
	token->is_heredoc_delimiter = 0;
	token->is_quoted_delimiter = 0;
	token->next = NULL;
	return (token);
}

void	add_token(t_token **head, t_token *new_token)
{
	t_token	*current;

	if (!*head)
	{
		*head = new_token;
		return ;
	}
	current = *head;
	while (current->next)
		current = current->next;
	current->next = new_token;
}
