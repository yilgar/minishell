#include "../minishell.h"

t_token	*find_token_position(t_token **tokens, t_token *token_to_split,
		t_token **prev_token)
{
	t_token	*current;

	*prev_token = NULL;
	current = *tokens;
	while (current && current != token_to_split)
	{
		*prev_token = current;
		current = current->next;
	}
	return (current);
}

void	link_token_to_list(t_token **tokens, t_token **current,
		t_token *prev_token, t_token *new_token)
{
	if (!*current)
	{
		if (prev_token)
			prev_token->next = new_token;
		else
			*tokens = new_token;
		*current = new_token;
	}
	else
	{
		(*current)->next = new_token;
		*current = new_token;
	}
}

t_token	*create_and_configure_token(t_gc *gc, char *word, int has_space)
{
	t_token	*new_token;

	new_token = create_token(gc, TOKEN_WORD, gc_track(gc, ft_strdup(word)));
	if (new_token)
	{
		new_token->has_space_before = has_space;
		new_token->is_heredoc_delimiter = 0;
	}
	return (new_token);
}

void	create_tokens_from_words(t_gc *gc, t_token **tokens, char **words,
		t_token *prev_token)
{
	t_token	*current;
	t_token	*new_token;
	int		i;

	i = 0;
	current = NULL;
	while (words[i])
	{
		if (words[i][0] != '\0')
		{
			new_token = create_and_configure_token(gc, words[i], (i > 0));
			if (new_token)
			{
				link_token_to_list(tokens, &current, prev_token, new_token);
				prev_token = NULL;
			}
		}
		i++;
	}
}

void	link_tokens_to_original(t_token **tokens, t_token *token_to_split)
{
	t_token	*current;

	current = *tokens;
	while (current && current->next != token_to_split->next)
		current = current->next;
	if (current)
		current->next = token_to_split->next;
}
