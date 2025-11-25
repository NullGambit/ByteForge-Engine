module forge.container.string_view;

import forge.container.string;

struct BaseStringView(C)
{
	
}

alias StringView = BaseStringView!char;