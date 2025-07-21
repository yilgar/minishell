#include "../minishell.h"

int	handle_redirection(t_gc *gc, t_cmd *cmd, t_token **tokens)
{
	t_token_type	type;
	char			*filename;
	t_token			*filename_token;
	int				result;

	result = 0;
	type = (*tokens)->type;
	if (!validate_redirection_token(tokens))
		return (0);
	if (type == TOKEN_REDIRECT_HEREDOC)
	{
		filename_token = *tokens;
		result = handle_heredoc_redirection(gc, cmd, filename_token);
		*tokens = (*tokens)->next;
	}
	else
	{
		filename = concatenate_tokens(gc, tokens);
		if (!filename)
			return (0);
		result = process_file_redirection(cmd, type, filename);
	}
	return (result);
}
