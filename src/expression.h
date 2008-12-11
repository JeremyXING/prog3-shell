#ifndef EXPRESSION_H
#define EXPRESSION_H


extern void tube(Expression * gauche, Expression * droite);
extern void redirection_stdout(Expression * e);
extern void redirection_stdin(Expression * e);
extern void redirection_stdout_append(Expression * e);
extern void redirection_stderr(Expression * e);
extern void redirection_stdout_stderr(Expression * e);
extern void sequence(Expression * e);
extern void sequence_et(Expression * e);
extern void sequence_ou(Expression * e);


#endif /*EXPRESSION_H */
