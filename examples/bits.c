#include <stdio.h>
#include <stc/cbitset.h>
#include <stc/cfmt.h>

int main() {
    cbitset_t set = cbitset_with_size(23, true);
    c_print(1, "count {}, {}\n", cbitset_count(set), set.size);

    cbitset_reset(&set, 9);
    cbitset_resize(&set, 43, false);
    c_withbuffer (str, char, set.size + 1)
        c_print(1, " str: {}\n", cbitset_to_str(set, str, 0, -1));

    c_print(1, "{:4}: ", set.size);
    c_forrange (i, int, set.size)
        c_print(1, "{}", cbitset_test(set, i));
    puts("");

    cbitset_set(&set, 28);
    cbitset_resize(&set, 77, true);
    cbitset_resize(&set, 93, false);
    cbitset_resize(&set, 102, true);
    cbitset_set_value(&set, 99, false);
    c_print(1, "{:4}: ", set.size);
    c_forrange (i, int, set.size)
        c_print(1, "{}", cbitset_test(set, i));

    puts("\nIterator:");
    c_print(1, "{:4}: ", set.size);
    c_foreach (i, cbitset, set)
        c_print(1, "{}", cbitset_itval(i));
    puts("");

    cbitset_t s2 = cbitset_clone(set);
    cbitset_flip_all(&s2);
    cbitset_set(&s2, 16);
    cbitset_set(&s2, 17);
    cbitset_set(&s2, 18);
    c_print(1, " new: ");
    c_forrange (i, int, s2.size)
        c_print(1, "{}", cbitset_test(s2, i));
    puts("");

    c_print(1, " xor: ");
    cbitset_xor_with(&set, s2);
    c_forrange (i, int, set.size)
        c_print(1, "{}", cbitset_test(set, i));
    puts("");

    cbitset_set_all(&set, false);
    c_print(1, "{:4}: ", set.size);
    c_forrange (i, int, set.size)
        c_print(1, "{}", cbitset_test(set, i));
    puts("");

    cbitset_del(&s2);
    cbitset_del(&set);
}