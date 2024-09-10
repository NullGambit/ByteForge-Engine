
field_count = int(input("Enter field count:"))

output = "#define FIELD_ENTRY(field) {#field, &field}\n"

for i in range(field_count):
    n = i + 1
    output += f"#define EXPAND_FIELD_{n}("

    fields = []

    if i > 0:
        for j in range(i+1):
            s = f"field{j+1}"
            fields.append(s)
            output += s
            if j < i:
                output += ", "
    else:
        fields.append("field")
        output += "field"
    output += ") "

    for j, field in enumerate(fields):
        output += f"FIELD_ENTRY({field})"

        if j < i:
            output += ", "
    output += '\n'

output += f"#define GET_EXPAND_FIELD_MACRO("

for i in range(field_count):
    output += f"_{i+1}"

    if i < field_count:
        output += ", "

output += "NAME, ...) NAME\n"

output += """#define EXPORT_FIELDS(...) \
std::vector<ecs::ComponentField> export_fields() override \
{ \
return { \
GET_EXPAND_FIELD_MACRO(__VA_ARGS__, """

for i in range(field_count):
    diff = field_count - i
    output += f"EXPAND_FIELD_{diff}"

    if i < field_count-1:
        output += ", "

output += ")(__VA_ARGS__) };} \\"

print(output)
