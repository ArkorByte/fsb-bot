#include "utils.text.hpp"

#include <string>
#include <vector>

/*
    Get full government type name.

    Tasks:
        1) Declare all government types.
        2) Return one depending on index.

    Parameters:
        - index / int / Government type from database.

    Returns:
        A string containing the government type name.
*/
std::string Text::get_government_type
(
    const int &index
)
{
    const std::vector<std::string> governments
    {
        "Presidential Republic",  "Parlimentary Republic",  "Federal Republic", "Monarchy",
        "Consitutional Monarchy", "Confederation",          "Military Rule",    "Anarchy",
        "Oligarchy",              "Aristocracy",            "One-Party State"
    };

    return governments[index];
}



/*
    Get full ideology name.

    Tasks:
        1) Declare all ideologies.
        2) Return one depending on index.

    Parameters:
        - index / int / Ideology from database.

    Returns:
        A string containing the ideology name.
*/
std::string Text::get_ideology
(
    const int &index
)
{
    const std::vector<std::string> ideologies
    {
        "Liberalism", "Conservatism", "Socialism",   "Communism", "Nationalism",
        "Centrism",   "Militarism",   "Imperialism", "Pacifism",  "Neutralism"
    };

    return ideologies[index];
}


/*
    Get full join condition name.

    Tasks:
        1) Declare all join conditions.
        2) Return one depending on index.

    Parameters:
        - index / int / Join condition from database.

    Returns:
        A string containing the join condition name.
*/
std::string Text::get_join_confition
(
    const int &index
)
{
    const std::vector<std::string> condition_values { "Opened", "On Invitation", "Closed" };

    return condition_values[index];
}



/*
    Get full rank name.

    Tasks:
        1) Declare all ranks.
        2) Return one depending on index.

    Parameters:
        - index / int / Rank from database/enum.

    Returns:
        A string containing the rank name.
*/
std::string Text::get_rank
(
    const int &index
)
{
    const std::vector<std::string> ranks
    {
        "Citizen", "Military", "Minister", "Prime Minister", "Head of State"
    };

    return ranks[index];
}



/*
    Get media freedom rating from score.

    Tasks:
        1) Check score value.
        2) Return rating.

    Parameters:
        - score / int / Media freedom score.

    Returns:
        A string containing the media freedom rating.
*/
std::string Text::get_media_freedom_rating
(
    const int &score
)
{
    if (score <= 25)
        return "Inexistant";
    else if (score <= 50)
        return "High censorship";
    else if (score <= 70)
        return "Complicated";
    else if (score <= 85)
        return "Good";
    else return "Very good";
}
