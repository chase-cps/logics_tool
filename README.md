# CHASE logics tool

The tool parses a set of assume-guarantee contracts expressed using the
logics-based language of CHASE, and provides a console to manipulate them.
The grammar of the language is in `grammar/LogicsContracts.g4`.

>**Usage**
>
> `logics_tool -i input_file [-o output_dir] [-c commands_file] [-V]`

> **Parameters:**
>
> - -i _Specifies the input file containing the contracts._
> - -o _Specifies the directory where the generated files are written._
> - -c _Specifies a file of commands (each terminated by `;`) to be executed
>   instead of opening the console._
> - -V _Enables verbose mode._

The console provides the following commands: `show`, `compose`, `conjunction`,
`disjunction`, `merging`, `quotient`, `separation`, `saturate`, `simplify`,
`refinement`, `verify`, and `synthesize`.
The `help` command shows their description.

For instance, from the `RoboticJoints` directory of the [demos][DEMOS]
repository:

`$> logics_tool -i specs.ltl -o /tmp/joints -c verification.chase`

`$> NuSMV /tmp/joints/refinement.smv`

The tool is also available as a Python module (`pychase_logicsLang`). It
requires the [core-library][CORE], the [chase-backends][BACKENDS], and the
ANTLR4 runtime provided in the [third_party][THIRD_PARTY] repository. For the
installation, please refer to the [CHASE website][WEB].

## License

This project is released under the 3-Clause BSD License.

[DEMOS]: https://github.com/chase-cps/demos
[CORE]: https://github.com/chase-cps/core-library
[BACKENDS]: https://github.com/chase-cps/chase-backends
[THIRD_PARTY]: https://github.com/chase-cps/third_party
[WEB]: https://chase-cps.github.io/doc/md/install.html
