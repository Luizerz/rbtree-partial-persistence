# Árvore Rubro-Negra com Persistência Parcial (Fat Node)

Implementação em C++ de uma Árvore Rubro-Negra (RBT) com **persistência parcial** usando o método **Fat Node**, conforme Driscoll, Sarnak, Sleator e Tarjan (1989). Cada operação de inserção ou remoção cria uma nova versão da estrutura, permitindo consultar qualquer versão histórica sem alterá-la.

---

## Autores

- Luiz Matheus Sena Macedo [603273]
- Leticia Sampaio Araújo Gonçalves [603266]

---

## Requisitos

### Linux (Ubuntu)
- Ubuntu 20.04 ou superior
- GCC 7 ou superior (`g++`) com suporte a C++17
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

---

## Compilação

```bash
make build
```

Para limpar os artefatos de compilação:

```bash
make clean
```

---

## Modo de uso

O programa recebe um arquivo de entrada `.txt` como argumento, imprime os resultados no terminal e grava tudo em `saida.txt`.

```bash
make run                              # usa entrada.txt por padrão
make run INPUT=outro_arquivo.txt      # arquivo de entrada personalizado
```

---

## Testes

O projeto inclui 30 casos de teste automatizados em `tests/`. Para executar todos:

```bash
make test
```

O resultado é impresso no terminal (`PASSED` / `FAILED`) para cada teste, seguido do total:

```
Running 30 tests...
  Test 01: PASSED
  Test 02: PASSED
  ...
Results: 30 passed, 0 failed
```

Cada teste é composto por um par de arquivos:

| Arquivo | Descrição |
|---|---|
| `tests/entradaXX.txt` | Arquivo de entrada com as operações |
| `tests/saida_esperadaXX.txt` | Saída esperada para aquela entrada |

Para testar um caso manualmente e inspecionar diferenças:

```bash
make run INPUT=tests/entrada05.txt
diff saida.txt tests/saida_esperada05.txt > diferencas.txt
```

---

## Formato do arquivo de entrada

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

## Descrição das funções e estruturas

### Estruturas

| Estrutura | Arquivo | Descrição |
|---|---|---|
| `Color` | `src/core/Node.hpp` | Enumeração `RED` / `BLACK` |
| `Field` | `src/core/Node.hpp` | Enumeração dos campos modificáveis: `LEFT`, `RIGHT`, `PARENT`, `COLOR` |
| `Mod` | `src/core/Node.hpp` | Entrada do histórico de modificações: `{version, field, value}` — `value` é `Node*` para ponteiros ou `Color` para a cor |
| `Node` | `src/core/Node.hpp` | Nó Fat Node com valores originais (`origLeft`, `origRight`, `origParent`, `origColor`), lista unificada de modificações (`mods`) e ponteiros de retorno (`backPointers`) |
| `ITreeCommands` | `src/core/PersistentRBTree.hpp` | Interface pura com as operações públicas da árvore |
| `Command` | `src/io/Command.hpp` | Resultado do parsing de uma linha de entrada |
| `CommandType` | `src/io/Command.hpp` | Enumeração `INC`, `REM`, `IMP`, `SUC`, `UNKNOWN` |

### Classes principais

| Classe | Arquivo | Descrição |
|---|---|---|
| `FatNodeStore` | `src/core/FatNodeStore.hpp/.cpp` | Gerencia a memória dos nós e implementa os acessores versionados (`getLeft`, `getRight`, `getParent`, `getColor`) via busca binária + varredura por campo, os setters com manutenção de `backPointers` e a lógica de overflow (`handleOverflow`) |
| `RBTreeEngine` | `src/core/RBTreeEngine.hpp/.cpp` | Contém toda a lógica do algoritmo Rubro-Negro: `rotateLeft`, `rotateRight`, `insertFixUp`, `deleteFixUp`, `transplant`, `searchVer`, `minVer` |
| `PersistentRBTree` | `src/core/PersistentRBTree.hpp/.cpp` | Implementa `ITreeCommands`; gerencia o array de raízes por versão e orquestra `insert`, `remove`, `printVersion` e `successor` |
| `CommandParser` | `src/io/CommandParser.hpp/.cpp` | Converte uma linha de texto em um `Command` |

### Funções livres / namespaces

| Função | Arquivo | Descrição |
|---|---|---|
| `TreeQuery::inorder` | `src/query/TreeFormatter.hpp/.cpp` | Percurso em ordem com profundidade e cor para o comando `IMP` |
| `TreeQuery::successor` | `src/query/SuccessorFinder.hpp/.cpp` | Busca do sucessor estrito em uma versão para o comando `SUC` |
| `emit` | `src/io/OutputWriter.hpp` | Escreve uma linha simultaneamente no arquivo `saida.txt` e no terminal |

---

## Implementação Fat Node

A persistência parcial segue fielmente o método de Driscoll et al. (1989):

### Constantes

| Constante | Valor | Significado |
|---|---|---|
| `D` | `6` | Limite de modificações por nó — `2 × p`, onde `p = 3` ponteiros (`left`, `right`, `parent`) |
| `MAX_VERSIONS` | `100` | Número máximo de versões suportadas |

### Estrutura do nó

Cada nó armazena:
- **Valores originais** (`origLeft`, `origRight`, `origParent`, `origColor`) — estado no momento da criação.
- **Lista unificada de mods** (`std::vector<Mod> mods`) — lista ordenada por versão crescente com entradas `{version, Field, value}`, onde `value` é `std::variant<Node*, Color>`.
- **Ponteiros de retorno** (`std::vector<Node*> backPointers`) — **não versionados** — contém todos os nós `m` que possuem algum campo apontando para este nó na versão mais recente.

### Leitura versionada — `getLeft(n, v)` etc.

1. Busca binária em `mods` para encontrar o último índice com `version ≤ v`.
2. Varredura reversa a partir desse índice para localizar o campo desejado (`Field::LEFT`, etc.).
3. Se nenhuma modificação for encontrada, retorna o valor original (`orig*`).

Complexidade: **O(log m + p)**, onde `m` = número de mods e `p` = número de campos.

### Escrita versionada — `setLeft(n, val, v)` etc. (Caso A)

1. Recupera o valor anterior do campo para atualizar os `backPointers`.
2. Remove `n` dos `backPointers` do nó antigo; adiciona `n` aos `backPointers` do novo nó.
3. Appenda `{v, Field::X, val}` à lista `mods`.
4. Se `mods.size() > D`: aciona `handleOverflow`.

### Overflow — `handleOverflow(old, v)` (Caso B)

Acionado quando um nó acumula mais de `D = 6` modificações:

1. **Cria cópia** `n'` com `mods = []` e `orig*` = estado atual de `old` (via getters).
2. **Passo 1 — herança de relacionamentos**: para cada nó que `old` apontava via `origLeft`, `origRight`, `origParent`, substitui `old` por `n'` nos `backPointers` desse nó.
3. **Passo 2 — redirecionamento**: itera sobre uma cópia de `old.backPointers` e, para cada nó `m` que apontava para `old`, atualiza o campo correspondente via setter (o que pode propagar recursivamente caso `m` também estoure).
4. Atualiza `roots[v] = n'` se `old` era a raiz.
5. `old` é preservado em memória para consultas a versões passadas.

---

## Referência

Driscoll, J. R., Sarnak, N., Sleator, D. D., & Tarjan, R. E. (1989).
*Making data structures persistent.*
Journal of Computer and System Sciences, 38(1), 86–124.
