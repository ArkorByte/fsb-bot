#include "utils.text.hpp"

#include <map>
#include <string>

/*

*/
std::string Text::get_nation_flag
(
    const std::string &nation_id
)
{
    std::map<std::string, std::string> flags
    {
        { "afghanistan", "🇦🇫" },              { "albania", "🇦🇱" },                          { "algeria", "🇩🇿" },           { "andorra", "🇦🇩" },
        { "antigua_and_barbuda", "🇦🇬" },      { "argentina", "🇦🇷" },                        { "armenia", "🇦🇲" },           { "australia", "🇦🇺" },
        { "austria", "🇦🇹" },                  { "belarus", "🇧🇾" },                          { "azerbaijan", "🇦🇿" },        { "bahamas", "🇧🇸" },
        { "bahrain", "🇧🇭" },                  { "bangladesh", "🇧🇩" },                       { "barbados", "🇧🇧" },          { "belarus", "🇧🇾" },
        { "belgium", "🇧🇪" },                  { "belize", "🇧🇿" },                           { "benin", "🇧🇯" },             { "bhutan", "🇧🇹" },
        { "bolivia", "🇧🇴" },                  { "bosnia_and_herzegovina", "🇧🇦" },           { "botswana", "🇧🇼" },          { "brazil", "🇧🇷" },
        { "brunei", "🇧🇳" },                   { "bulgaria", "🇧🇬" },                         { "burkina_faso", "🇧🇫" },      { "burundi", "🇧🇮" },
        { "cape_verde", "🇨🇻" },               { "cambodia", "🇰🇭" },                         { "cameroon", "🇨🇲" },          { "canada", "🇨🇦" },
        { "central_african_republic", "🇨🇫" }, { "chad", "🇹🇩" },                             { "chile", "🇨🇱" },             { "china", "🇨🇳" },
        { "colombia", "🇨🇴" },                 { "comoros", "🇰🇲" },                          { "congo_brazzaville", "🇨🇬" }, { "costa_rica", "🇨🇷" },
        { "cote_divoire", "🇨🇮" },             { "croatia", "🇭🇷" },                          { "cuba", "🇨🇺" },              { "cyprus", "🇨🇾" },
        { "czechia", "🇨🇿" },                  { "denmark", "🇩🇰" },                          { "djibouti", "🇩🇯" },          { "dominica", "🇩🇲" },
        { "dominican_republic", "🇩🇴" },       { "dr_congo", "🇨🇩" },                         { "ecuador", "🇪🇨" },           { "egypt", "🇪🇬" },
        { "el_salvador", "🇸🇻" },              { "equatorial_guinea", "🇬🇶" },                { "eritrea", "🇪🇷" },           { "estonia", "🇪🇪" },
        { "eswatini", "🇸🇿" },                 { "ethiopia", "🇪🇹" },                         { "fiji", "🇫🇯" },              { "finland", "🇫🇮" },
        { "france", "🇫🇷" },                   { "gabon", "🇬🇦" },                            { "gambia", "🇬🇲" },            { "georgia", "🇬🇪" },
        { "germany", "🇩🇪" },                  { "ghana", "🇬🇭" },                            { "greece", "🇬🇷" },            { "grenada", "🇬🇩" },
        { "guatemala", "🇬🇹" },                { "guinea", "🇬🇳" },                           { "guinea_bissau", "🇬🇼" },     { "guyana", "🇬🇾" },
        { "haiti", "🇭🇹" },                    { "honduras", "🇭🇳" },                         { "hungary", "🇭🇺" },           { "iceland", "🇮🇸" },
        { "india", "🇮🇳" },                    { "indonesia", "🇮🇩" },                        { "iran", "🇮🇷" },              { "iraq", "🇮🇶" },
        { "ireland", "🇮🇪" },                  { "israel", "🇮🇱" },                           { "italy", "🇮🇹" },             { "jamaica", "🇯🇲" },
        { "japan", "🇯🇵" },                    { "jordan", "🇯🇴" },                           { "kazakhstan", "🇰🇿" },        { "kenya", "🇰🇪" },
        { "kiribati", "🇰🇮" },                 { "kuwait", "🇰🇼" },                           { "kyrgyzstan", "🇰🇬" },        { "laos", "🇱🇦" },
        { "latvia", "🇱🇻" },                   { "lebanon", "🇱🇧" },                          { "lesotho", "🇱🇸" },           { "liberia", "🇱🇷" },
        { "libya", "🇱🇾" },                    { "liechtenstein", "🇱🇮" },                    { "lithuania", "🇱🇹" },         { "luxembourg", "🇱🇺" },
        { "madagascar", "🇲🇬" },               { "malawi", "🇲🇼" },                           { "malaysia", "🇲🇾" },          { "maldives", "🇲🇻" },
        { "mali", "🇲🇱" },                     { "malta", "🇲🇹" },                            { "marshall_islands", "🇲🇭" },  { "mauritania", "🇲🇷" },
        { "mauritius", "🇲🇺" },                { "mexico", "🇲🇽" },                           { "micronesia", "🇫🇲" },        { "moldova", "🇲🇩" },
        { "monaco", "🇲🇨" },                   { "mongolia", "🇲🇳" },                         { "montenegro", "🇲🇪" },        { "morocco", "🇲🇦" },
        { "mozambique", "🇲🇿" },               { "myanmar", "🇲🇲" },                          { "namibia", "🇳🇦" },           { "nauru", "🇳🇷" },
        { "nepal", "🇳🇵" },                    { "netherlands", "🇳🇱" },                      { "new_zealand", "🇳🇿" },       { "nicaragua", "🇳🇮" },
        { "niger", "🇳🇪" },                    { "nigeria", "🇳🇬" },                          { "north_korea", "🇰🇵" },       { "north_macedonia", "🇲🇰" },
        { "norway", "🇳🇴" },                   { "oman", "🇴🇲" },                             { "pakistan", "🇵🇰" },          { "palau", "🇵🇼" },
        { "panama", "🇵🇦" },                   { "papua_new_guinea", "🇵🇬" },                 { "paraguay", "🇵🇾" },          { "peru", "🇵🇪" },
        { "philippines", "🇵🇭" },              { "poland", "🇵🇱" },                           { "portugal", "🇵🇹" },          { "qatar", "🇶🇦" },
        { "romania", "🇷🇴" },                  { "russia", "🇷🇺" },                           { "rwanda", "🇷🇼" },            { "saint_kitts_and_nevis", "🇰🇳" },
        { "saint_lucia", "🇱🇨" },              { "saint_vincent_and_the_grenadines", "🇻🇨" }, { "samoa", "🇼🇸" },             { "san_marino", "🇸🇲" },
        { "sao_tome_and_principe", "🇸🇹" },    { "saudi_arabia", "🇸🇦" },                     { "senegal", "🇸🇳" },           { "serbia", "🇷🇸" },
        { "seychelles", "🇸🇨" },               { "sierra_leone", "🇸🇱" },                     { "singapore", "🇸🇬" },         { "slovakia", "🇸🇰" },
        { "slovenia", "🇸🇮" },                 { "solomon_islands", "🇸🇧" },                  { "somalia", "🇸🇴" },           { "south_africa", "🇿🇦" },
        { "south_korea", "🇰🇷" },              { "south_sudan", "🇸🇸" },                      { "spain", "🇪🇸" },             { "sri_lanka", "🇱🇰" },
        { "sudan", "🇸🇩" },                    { "suriname", "🇸🇷" },                         { "sweden", "🇸🇪" },            { "switzerland", "🇨🇭" },
        { "syria", "🇸🇾" },                    { "tajikistan", "🇹🇯" },                       { "tanzania", "🇹🇿" },          { "thailand", "🇹🇭" },
        { "timor_leste", "🇹🇱" },              { "togo", "🇹🇬" },                             { "tonga", "🇹🇴" },             { "trinidad_and_tobago", "🇹🇹" },
        { "tunisia", "🇹🇳" },                  { "turkey", "🇹🇷" },                           { "turkmenistan", "🇹🇲" },      { "tuvalu", "🇹🇻" },
        { "uganda", "🇺🇬" },                   { "ukraine", "🇺🇦" },                          { "uae", "🇦🇪" },               { "uk", "🇬🇧" },
        { "usa", "🇺🇸" },                      { "uruguay", "🇺🇾" },                          { "uzbekistan", "🇺🇿" },        { "vanuatu", "🇻🇺" },
        { "vatican_city", "🇻🇦" },             { "venezuela", "🇻🇪" },                        { "vietnam", "🇻🇳" },           { "yemen", "🇾🇪" },
        { "zambia", "🇿🇲" },                   { "zimbabwe", "🇿🇼" }
    };

    return flags[nation_id];
}
