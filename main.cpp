#include <chaiscript/chaiscript.hpp>

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

int main()
{
    std::string initialization_script_str;

    // Data
    double const mu_B = 0.057883817555; // The Bohr Magneton [meV/T]
    std::vector<Atom> basis_cell_atoms{
        Atom{ 0, 0, 0 },
    };

    // Data manipulation API
    auto add_atom = [&]( double x, double y, double z ) { basis_cell_atoms.push_back( { x, y, z } ); };

    // Setup the chaiscript namespace
    chaiscript::ChaiScript initialization_script;
    initialization_script.register_namespace(
        [&]( chaiscript::Namespace & spirit ) {
            spirit["mu_B"]     = chaiscript::const_var( mu_B );
            spirit["add_atom"] = chaiscript::var( chaiscript::fun( add_atom ) );
        },
        "spirit" );

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
        initialization_script( initialization_script_str );
    }
    catch( const std::exception & e )
    {
        fmt::print( "chaiscript exception: {}\n", e.what() );
    }
    for( auto & atom : basis_cell_atoms )
        fmt::print( "atom: {} {} {}\n", atom.x, atom.y, atom.z );
    fmt::print( "------------------------------------------\n" );

    return 0;
}