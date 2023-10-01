$$
\begin{align}
[\text{prog}] &\to [\text{stmt}]^* \\
[\text{stmt}] &\to 
\begin{cases}
\text{exit}([\text{expr}]); \\
\text{let}\space\text{indent} = [\text{expr}];
\end{cases}
\\
% [\text{exit}] &\to exit\space[\text{expr}]; \\
[\text{expr}] &\to 
\begin{cases}
\text{int\_lit} \\
\text{ident}
\end{cases}
\end{align}
$$