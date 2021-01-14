#include <chaiscript/chaiscript.hpp>
#include <chaiscript/extras/math.hpp>

#if defined( __cplusplus ) && __cplusplus >= 201703L && defined( __has_include )
#if __has_include( <filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;
#endif

#include <fmt/format.h>

#include <fstream>
#include <iomanip>
#include <streambuf>
#include <string>
#include <vector>

struct Atom
{
    double x = 0;
    double y = 0;
    double z = 0;
};

struct Pair
{
    int i = 0;
    int j = 0;

    int da = 0;
    int db = 0;
    int dc = 0;
};

struct ExchangePair
{
    int i;
    int j;
    int da;
    int db;
    int dc;
    float Jij;
};

int main()
{
    std::string initialization_script_str;

    // Constants
    double const mu_B = 0.057883817555;    // The Bohr Magneton [meV/T]
    double const mu_0 = 2.0133545 * 1e-28; // The vacuum permeability [T^2 m^3 / meV]

    // Geometry
    std::vector<Atom> basis_cell_atoms{
        Atom{ 0, 0, 0 },
    };
    std::array<int, 3> n_cells{ 1, 1, 1 };

    // Hamiltonian
    bool dynamic_field = false;
    std::function<double( double x, double y, double z, double t )> dynamic_field_function
        = []( double x, double y, double z, double t ) { return 3.0; };

    // Data manipulation API
    auto add_atom           = [&]( double x, double y, double z ) { basis_cell_atoms.push_back( { x, y, z } ); };
    auto set_n_cells        = [&]( int na, int nb, int nc ) { n_cells = { na, nb, nc }; };
    auto set_external_field = [&]( double magnitude, double direction[3] ) {};
    auto set_dynamic_field  = [&]( const std::function<double( double x, double y, double z, double t )> & function ) {
        dynamic_field          = true;
        dynamic_field_function = function;
    };
    auto set_exchange_shells = [&]( int n_shells, double * Jij ) { fmt::print( "not yet implemented\n" ); };
    auto add_exchange_pair   = [&]( int i, int j, int da, int db, int dc, double Jij ) {};

    // Setup the chaiscript namespaces
    chaiscript::ChaiScript initialization_script;

    auto mathlib = chaiscript::extras::math::bootstrap();
    initialization_script.add( mathlib );

    initialization_script.register_namespace(
        [&]( chaiscript::Namespace & ns ) {
            ns["mu_B"] = chaiscript::const_var( mu_B );
            ns["mu_0"] = chaiscript::const_var( mu_B );
        },
        "constants" );
    initialization_script.register_namespace(
        [&]( chaiscript::Namespace & ns ) {
            ns["add_atom"]    = chaiscript::var( chaiscript::fun( add_atom ) );
            ns["set_n_cells"] = chaiscript::var( chaiscript::fun( set_n_cells ) );
        },
        "geometry" );
    initialization_script.register_namespace(
        [&]( chaiscript::Namespace & ns ) {
            ns["set_external_field"]  = chaiscript::var( chaiscript::fun( set_external_field ) );
            ns["set_dynamic_field"]   = chaiscript::var( chaiscript::fun( set_dynamic_field ) );
            ns["set_exchange_shells"] = chaiscript::var( chaiscript::fun( set_exchange_shells ) );
            ns["add_exchange_pair"]   = chaiscript::var( chaiscript::fun( add_exchange_pair ) );
        },
        "hamiltonian_heisenberg" );

    // Read the initialization script file
    auto input_folder = fs::path( "input" );
    if( fs::is_directory( input_folder ) )
    {
        auto input_file = input_folder / "initialize.chai";
        if( fs::is_regular_file( input_file ) )
        {
            // Read input file
            std::ifstream ifs( input_file );

            ifs.seekg( 0, std::ios::end );
            initialization_script_str.reserve( ifs.tellg() );
            ifs.seekg( 0, std::ios::beg );

            initialization_script_str.assign(
                ( std::istreambuf_iterator<char>( ifs ) ), std::istreambuf_iterator<char>() );
        }
    }

    // Execute
    fmt::print( "------------------------------------------\n" );
    try
    {
        initialization_script.import( "constants" );
        initialization_script.import( "geometry" );
        initialization_script.import( "hamiltonian_heisenberg" );
        initialization_script( initialization_script_str );
    }
    catch( const std::exception & e )
    {
        fmt::print( "chaiscript exception: {}\n", e.what() );
    }
    catch( ... )
    {
        fmt::print( "other exception: ...\n" );
    }

    fmt::print( "number of atoms in the basis cell: {}\n", basis_cell_atoms.size() );
    int i = 0;
    for( auto & atom : basis_cell_atoms )
    {
        fmt::print( "    atom {}: ({:^10.3f} {:^10.3f} {:^10.3f})\n", i + 1, atom.x, atom.y, atom.z );
        ++i;
    }
    fmt::print( "n_cells: {} {} {}\n", n_cells[0], n_cells[1], n_cells[2] );

    fmt::print( "time-dependent external magnetic field:\n" );
    for( int it = 0; it < 10; ++it )
    {
        double t = it * 0.1;
        fmt::print( "field(t = {:^5.2f}) = {}\n", t, dynamic_field_function( 0, 0, 0, t ) );
    }
    fmt::print( "------------------------------------------\n" );

    return 0;
}