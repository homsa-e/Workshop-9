#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <map>
#include <algorithm>

std::ofstream out("population.txt");

enum worldcities_data_columns
{
    city = 0,
    city_ascii,
    lat,
    lng,
    country,
    iso2,
    iso3,
    admin_name,
    capital,
    population,
    id,
    total_columns
};

std::string no_quotes (const std::string &input)
{
    std::string result = input;
    result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
    return result;
}

struct city_data
{
    city_data ()
    {}

    city_data (const std::vector <std::string> &tokens) :
        lat (std::stod (no_quotes (tokens[worldcities_data_columns::lat]))),
        lng (std::stod (no_quotes (tokens[worldcities_data_columns::lng]))),
        country (no_quotes (tokens[worldcities_data_columns::country])),
        is_capital (no_quotes (tokens[worldcities_data_columns::capital]) == "primary")
    {
        std::string population_string = no_quotes (tokens[worldcities_data_columns::population]);
        if (population_string.empty ())
        {
            has_population = false;
        }
        else
        {
            has_population = true;
            population = std::stod (population_string);
        }
    }
    std::string city;
    double lat;
    double lng;
    double population;
    bool has_population;
    std::string country;
    bool is_capital;
};

std::unordered_map<std::string, city_data> worldCities ()
{
    std::ifstream in ("worldcities.csv");
    std::string line;

    std::unordered_map<std::string, city_data> result;

    std::getline (in, line);
    while (!in.eof() && in.good ())
    {
      std::getline (in, line);

      std::vector<std::string> tokens;
      std::stringstream ss(line);
      std::string token;
      while (!ss.eof () && ss.good ())
      {
          std::getline (ss, token, ',');

          if (!tokens.empty () &&
              !tokens[tokens.size () - 1].empty () &&

               tokens[tokens.size () - 1].front () == '"'
               &&
               (tokens[tokens.size () - 1].back () != '"' ||
                tokens[tokens.size () - 1].size () == 1))
          {
              tokens[tokens.size () - 1] += "," + token;
          }
          else
          {
              tokens.push_back (token);
          }
      }

      if (tokens.size () == total_columns)
        result[tokens[worldcities_data_columns::city_ascii]] = city_data (tokens);
      else
        assert (ss.eof () || !ss.good ());
    }
    return result;
}

size_t howManyCountries (const std::unordered_map<std::string, city_data> &world_cities)
{
    std::unordered_set<std::string> countries;
    for (const std::pair<const std::string, city_data> &data : world_cities)
    {
        countries.insert (data.second.country);
    }
    return countries.size ();
}

void Capitals(const std::unordered_map<std::string, city_data> &world_cities) {
    for (const std::pair<const std::string, city_data> &data : world_cities){
        if (data.second.is_capital){
            std::cout << data.second.country << ": " << data.first << std::endl;
        }
    }
}

std::vector <int> uninhabitedLongitude(const std::unordered_map<std::string, city_data> &world_cities){
    std::unordered_set<int> longs;
    std::vector <int> ans;
    for (const std::pair<const std::string, city_data> &data : world_cities){
        longs.insert(int(data.second.lng));
    }
    for (int i = -179; i < 180; i++){
        if (longs.find(i) == longs.end())
            ans.push_back(i);
    }
    return ans;
}

void popul(const std::unordered_map<std::string, city_data> &world_cities) {
    std::map<std::string, std::map <std::string, int> > ans;
    std::map <std::string, int> emp;
    std::map<std::string, int> country_population;
    for (const std::pair<const std::string, city_data> &data : world_cities){
        ans[data.second.country][data.first] = data.second.population;
        if (country_population.find(data.second.country) != country_population.end()){
            country_population[data.second.country] += data.second.population;
        }
        else{
            country_population[data.second.country] = data.second.population;
        }
    }
    for (const std::pair <const std::string, std::map <std::string, int> > &data : ans){
         out << data.first << ": ";
         if (country_population[data.first] == 0)
             out << "?" << std::endl;
         else
             out << country_population[data.first] << std::endl;
         for (const std::pair <std::string, int> &city : data.second){
             out << "-> " << no_quotes(city.first) << ": ";
             if (city.second == 0)
                 out << "?" << " / ";
             else
                 out << city.second << " / ";
             if (country_population[data.first] == 0)
                 out << "?" << std::endl;
             else\
                 out << country_population[data.first] << std::endl;
         }
    }
}

int main ()
{
  std::unordered_map<std::string, city_data> world_cities = worldCities ();
  size_t number_of_countries = howManyCountries (world_cities);
  std::vector <int> uninhlngs = uninhabitedLongitude(world_cities);
  std::cout << number_of_countries << std::endl;
  Capitals(world_cities);
  for (int i : uninhlngs){
      std::cout << i << std::endl;
  }
  popul(world_cities);
  return 0;
}
