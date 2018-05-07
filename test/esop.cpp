#include <catch.hpp>
#include <esop/esop.hpp>
#include <esop/synthesis.hpp>
#include <esop/exact_synthesis.hpp>

/**
 * Synthesize one ESOP for a given Boolean function
 */
TEST_CASE( "one_esop", "[synthesis]" )
{
  nlohmann::json config;
  config[ "maximum_cubes" ] = 10;
  config[ "one_esop" ] = true;

  // const auto number_of_variables = 5u;
  const std::string bits = "01111111111101010111111101010011";
  const std::string care = "11111111111111111111111111111111";

  const auto esops = esop::exact_synthesis_from_binary_string( bits, care, config );

  /* number of ESOPs */
  CHECK( esops.size() == 1 );

  /* number of terms of first ESOP */
  for ( const auto& e : esops )
  {
    // esop::print_esop_as_exprs( e, number_of_variables );
    CHECK( esop::verify_esop( e, bits, care ) );
  }
}

/**
 * Synthesize all ESOPs for a given Boolean function
 */
TEST_CASE( "all_esop", "[synthesis]" )
{
  nlohmann::json config;
  config[ "maximum_cubes" ] = 10;
  config[ "one_esop" ] = false;

  // const auto number_of_variables = 5u;
  const std::string bits = "01111111111101010111111101010011";
  const std::string care = "11111111111111111111111111111111";

  const auto esops = esop::exact_synthesis_from_binary_string( bits, care, config );

  /* number of ESOPs */
  CHECK( esops.size() == 7 );

  /* number of terms of first ESOP */
  for ( const auto& e : esops )
  {
    // esop::print_esop_as_exprs( e, number_of_variables );
    CHECK( esop::verify_esop( e, bits, care ) );
  }
}

/**
 * Synthesize one ESOP for a given Boolean function (new API)
 */
TEST_CASE( "new_api_synthesize", "[synthesis]" )
{
  /* specification */
  esop::spec s{ "01111111111101010111111101010011", "11111111111111111111111111111111" };

  /* synthesizer */
  esop::simple_synthesizer synth( s );
  esop::simple_synthesizer_params params;

  /* try to synthesize with 4 terms which is *not* enough */
  params.number_of_terms = 4;
  auto esop = synth.synthesize( params );
  CHECK( esop.empty() );

  /* try again with 5 terms */
  params.number_of_terms = 5;
  esop = synth.synthesize( params );

  auto number_of_variables = 5u;
  // esop::print_esop_as_exprs( esop, number_of_variables );
  CHECK( esop::verify_esop( esop, s.bits, s.care ) );
}

/**
 * Synthesize a minimum ESOP for a given Boolen function (new API)
 */
TEST_CASE( "new_api_minimize", "[synthesis]" )
{
  /* specification */
  esop::spec s{ "01111111111101010111111101010011", "11111111111111111111111111111111" };

  /* synthesizer */
  esop::minimum_synthesizer synth( s );
  esop::minimum_synthesizer_params params;

  esop::esop_t esop;

  /* search downwards */
  params.begin = 8;
  params.next = [&]( int& i, bool sat ){ if ( i <= 0 || !sat ) return false; --i; return true; };
  esop = synth.synthesize( params );
  CHECK( !esop.empty() );
  CHECK( esop.size() == 5u );

  /* search upwards */
  params.begin = 1;
  params.next = [&]( int& i, bool sat ){ if ( i >= 8 || sat ) return false; ++i; return true; };
  esop = synth.synthesize( params );
  CHECK( !esop.empty() );
  CHECK( esop.size() == 5u );
}