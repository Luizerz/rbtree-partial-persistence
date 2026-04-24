# Árvore Rubro-Negra com Persistência Parcial (Fat Node)

Implementação em C++ de uma Árvore Rubro-Negra (RBT) com **persistência parcial** usando o método **Fat Node**, conforme Driscoll, Sarnak, Sleator e Tarjan (1989). Cada operação de inserção ou remoção cria uma nova versão da estrutura, permitindo consultar qualquer versão histórica sem alterá-la.

---

## Requisitos

### Linux (Ubuntu)
- Ubuntu 20.04 ou superior
- GCC 7 ou superior
- Make

```bash
sudo apt update && sudo apt install g++ make
```

### Windows
- **Opção A — WSL2** (recomendado): instale o [WSL2](https://learn.microsoft.com/pt-br/windows/wsl/install) com Ubuntu e siga os passos do Linux acima.
- **Opção B — MinGW-w64**: instale via [MSYS2](https://www.msys2.org/), depois execute no terminal MSYS2:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc make
```

- **Opção C — Visual Studio 2019+**: abra `main.cpp` diretamente (suporte a C++17 nativo; não requer `make`).

---

## Compilação

```bash
make
```

Para limpar os artefatos:

```bash
make clean
```

---

## Modo de uso

O programa lê operações de `entrada.txt`, imprime resultados no terminal e grava tudo em `saida.txt`.

```bash
./rbn-tree
```

No Windows (MinGW):
```bash
rbn-tree.exe
```

---

## Formato do arquivo de entrada (`entrada.txt`)

Cada linha contém uma operação. Linhas em branco são ignoradas.

### `INC <valor>`
Insere o inteiro na estrutura e cria uma nova versão. Não produz saída.

```
INC 50
```

### `REM <valor>`
Remove o inteiro da estrutura e cria uma nova versão. Se o valor não existir, a estrutura não é alterada, mas uma nova versão ainda é criada. Não produz saída.

```
REM 13
```

### `IMP <versão>`
Imprime todos os nós da versão indicada em ordem crescente, com profundidade e cor. Se a versão não existir, usa a mais recente. Não cria nova versão.

```
IMP 5
```

Saída:
```
IMP 5
13,2,R 42,1,N 50,0,N 52,2,R 65,1,N ;
```

Formato de cada nó: `valor,profundidade,cor` — cor `R` = vermelho, `N` = negro (preto).
A profundidade da raiz é `0`.

### `SUC <valor> <versão>`
Retorna o sucessor do valor (menor inteiro na estrutura **estritamente maior** que o valor fornecido) na versão indicada. Se a versão não existir, usa a mais recente. Se não houver sucessor, imprime `infinito`. Não cria nova versão.

```
SUC 40 5
```

Saída:
```
SUC 40 5 42
```

---

## Exemplo completo

**`entrada.txt`**
```
INC 50
INC 42
INC 65
INC 52
INC 13
IMP 5
REM 13
IMP 6
IMP 5
SUC 40 5
SUC 60 6
SUC 99 5
```

**Saída (terminal e `saida.txt`)**
```
IMP 5
13,2,R 42,1,N 50,0,N 52,2,R 65,1,N ;
IMP 6
42,1,N 50,0,N 52,2,R 65,1,N ;
IMP 5
13,2,R 42,1,N 50,0,N 52,2,R 65,1,N ;
SUC 40 5 42
SUC 60 6 65
SUC 99 5 infinito
```

Note que `IMP 5` após a remoção ainda mostra o nó `13` — a versão 5 é preservada intacta.

---

## Versões

- A estrutura começa na **versão 0** (árvore vazia).
- Cada `INC` ou `REM` incrementa a versão em 1.
- Máximo de **100 versões** (versões 0 a 99).
- `IMP` e `SUC` não criam novas versões.

---

## Referência

Driscoll, J. R., Sarnak, N., Sleator, D. D., & Tarjan, R. E. (1989).
*Making data structures persistent.*
Journal of Computer and System Sciences, 38(1), 86–124.
