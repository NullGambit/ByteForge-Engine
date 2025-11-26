module forge.container.string_view;

import forge.container.string;
import forge.container.contigious;
import forge.container;

struct BaseStringView(T)
{
	mixin ContigiousCore!T;
	mixin ContigiousRead!T;
	mixin StringRead!T;

	this(S)(const auto ref S str)
	if (StringLike!S)
	{
		ptr = str.ptr;
		m_length = str.length;
	}
}

alias StringView = BaseStringView!(const(char));
