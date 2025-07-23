#include "../minishell.h"

void	split_token_by_whitespace(t_gc *gc, t_token **tokens,
		t_token *token_to_split)
{
	char	**words;
	t_token	*prev_token;

	if (!token_to_split || !token_to_split->value
		|| !ft_strchr(token_to_split->value, ' '))
		return ;
	find_token_position(tokens, token_to_split, &prev_token);
	words = ft_split(token_to_split->value, ' ');
	if (!words)
		return ;
	gc_track_array(gc, (void **)words);
	gc_track(gc, words);
	create_tokens_from_words(gc, tokens, words, prev_token);
	link_tokens_to_original(tokens, token_to_split);
}

void	mark_heredoc_delimiters(t_token *tokens)
{
	t_token	*current;

	current = tokens;
	while (current && current->type != TOKEN_EOF)
	{
		if (current->type == TOKEN_REDIRECT_HEREDOC && current->next)
			current->next->is_heredoc_delimiter = 1;
		current = current->next;
	}
}

void	set_token_properties(t_token *token, char *value, t_token_type type)
{
	token->value = value;
	token->type = type;
}

void	expand_env_var_token(t_gc *gc, t_env *env, t_token *current)
{
	char	*var_name;
	char	*expanded;
	char	*new_value;

	var_name = current->value + 1;
	if (!var_name || var_name[0] == '\0')
		new_value = gc_track(gc, ft_strdup("$"));
	else
	{
		expanded = env_get_value(env, var_name);
		if (expanded)
			new_value = gc_track(gc, ft_strdup(expanded));
		else
			new_value = gc_track(gc, ft_strdup(""));
	}
	set_token_properties(current, new_value, TOKEN_WORD);
	current->from_env_expansion = 1;
}

void	expand_exit_status_token(t_gc *gc, t_token *current)
{
	char	*new_value;

	new_value = gc_track(gc, ft_itoa(g_exit_status));
	set_token_properties(current, new_value, TOKEN_WORD);
}
