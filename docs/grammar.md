$$
\begin{align}
[\text{Prog}] &\to [\text{Stmt}]^* \\
[\text{Stmt}] &\to 
\begin{cases}
\text{exit}([\text{Expr}]); \\
\text{let}\space\text{indent} = [\text{Expr}];
\end{cases}
\\
% [\text{exit}] &\to exit\space[\text{expr}]; \\
[\text{Expr}] &\to 
\begin{cases}
% \text{int\_lit} \\
[\text{Term}] \\
% \text{ident} \\
[\text{NodeBinExpr}]
\end{cases} \\
[\text{BinExpr}] &\to
\begin{cases}
[\text{Expr}] / [\text{Expr}] & \text{prec} = 1 \\
[\text{Expr}] * [\text{Expr}] & \text{prec} = 1 \\
[\text{Expr}] + [\text{Expr}] & \text{prec} = 0 \\
[\text{Expr}] - [\text{Expr}] & \text{prec} = 0 \\
\end{cases} \\
[\text{Term}] &\to
\begin{cases}
\text{int\_lit} \\
\text{ident} \\
\end{cases}
\end{align}
$$