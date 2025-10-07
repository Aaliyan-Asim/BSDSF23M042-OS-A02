---

# 🧾 REPORT.md

## **Feature 2 – Version 1.1.0: Complete Long Listing Format**

---

### **Q1. What is the crucial difference between the `stat()` and `lstat()` system calls? When is `lstat()` more appropriate for `ls`?**

Both `stat()` and `lstat()` are system calls used to obtain metadata about files, such as permissions, ownership, size, and timestamps.

* **`stat()`**:
  Follows symbolic links and retrieves information about the **target file** that the link points to.

* **`lstat()`**:
  Retrieves information about the **link itself**, not the target file.

✅ **In the context of `ls`:**
When implementing the `ls -l` option, we use `lstat()` because it allows us to correctly display details about symbolic links (like their name and target path) rather than the file they point to. This ensures that links are represented accurately in the output (with the leading `l` type indicator).

---

### **Q2. How does the `st_mode` field store file type and permissions, and how can bitwise operators and macros extract this information?**

The `st_mode` field in the `struct stat` structure is an integer bitmask that encodes both the **file type** and **permission bits**.

You can use **bitwise AND (`&`)** with predefined macros to check specific bits in `st_mode`.

#### 🔹 Example:

```c
if (st_mode & S_IFDIR) {
    printf("This is a directory.\n");
}

if (st_mode & S_IRUSR) {
    printf("Owner has read permission.\n");
}
```

#### 🔹 Explanation:

* `S_IFDIR`, `S_IFREG`, etc. identify the **file type** (directory, regular file, symbolic link, etc.).
* `S_IRUSR`, `S_IWUSR`, `S_IXUSR`, etc. represent **user permissions** for read, write, and execute.
* The bitwise AND operation (`&`) checks whether specific bits are set, allowing you to interpret the encoded information in `st_mode`.

---

### ✅ **Summary**

| Concept                   | Explanation                                                             |
| ------------------------- | ----------------------------------------------------------------------- |
| **`stat()` vs `lstat()`** | `stat()` follows symbolic links; `lstat()` does not.                    |
| **`st_mode` field**       | Stores both file type and permission bits.                              |
| **Bitwise check**         | Use `&` with macros like `S_IFDIR` or `S_IRUSR` to extract information. |

---

### 🧾 **Feature 3 – Version 1.2.0: Column Display**

**Q1. How does the program decide how many columns and rows to use?**
The program first scans all filenames in the target directory to find the length of the longest name.
It then uses the `ioctl()` system call with `TIOCGWINSZ` to obtain the terminal width (number of columns).
Using that information, it computes:

```
col_width = max_filename_length + spacing
num_cols  = terminal_width / col_width
num_rows  = ceil(total_files / num_cols)
```

If the terminal width cannot be detected, it defaults to 80 characters.

---

**Q2. Why is `ioctl()` used, and how does it help?**
`ioctl()` (I/O control) lets a program query or configure device-specific parameters.
When used with `TIOCGWINSZ`, it returns the current terminal’s width and height, allowing `ls` to automatically adapt its output formatting to the user’s screen size.

---

**Q3. What does “down then across” mean in this context?**
Instead of printing filenames row-by-row from left to right, the program fills each column vertically first.
Example for 9 files and 3 columns:

```
file1  file4  file7
file2  file5  file8
file3  file6  file9
```

This mimics the default behavior of the real `ls` command.

---

**Q4. How is spacing and alignment handled?**
Each printed filename is left-aligned in a fixed-width column using:

```c
printf("%-*s", col_width, filenames[index]);
```

The `-*` width specifier pads filenames with spaces so columns remain aligned regardless of varying filename lengths.

---

**Q5. What dynamic-memory strategy is used?**
All filenames are stored in a dynamically allocated array (`malloc` + `realloc`), which grows as needed.
After printing, the program frees every string and the main array to prevent memory leaks.

---



---

## Version 1.3.0 — Horizontal Column Display (-x)

### Q1. Compare the implementation complexity of the "down then across" (vertical) printing logic versus the "across" (horizontal) printing logic. Which one requires more pre-calculation and why?

The **"down then across" (vertical)** printing logic is more complex because it requires pre-calculating both the number of columns and rows, then indexing the filenames in a two-dimensional layout (using the formula `index = column * num_rows + row`). This means you need to carefully compute how many items fit vertically in each column before printing.

In contrast, the **"across" (horizontal)** logic simply loops through the filenames from left to right and wraps when the terminal width limit is reached. It doesn’t require calculating the number of rows or managing complex indexing — just checking if the next filename exceeds the current terminal width.

Therefore, **the vertical layout requires more pre-calculation** and is logically trickier to implement due to the need for coordinated row/column mapping.

---

### Q2. Describe the strategy you used in your code to manage the different display modes (-l, -x, and default). How did your program decide which function to call for printing?

A simple **state management approach** was used through flag variables:


- A `long_flag` is set when `-l` is detected.
- A `horizontal_flag` is set when `-x` is detected.

In the `main()` function, after parsing all command-line arguments, the program decides which display mode to use:

```c
if (long_flag)
    print_long_listing(dir);
else if (horizontal_flag)
    print_horizontal_listing(dir);
else
    print_column_listing(dir);

## Version 1.4.0 – Alphabetical Sort

**Q1:** Why is it necessary to read all directory entries into memory before you can sort them?  
What are the potential drawbacks of this approach for directories containing millions of files?

**Q2:** Explain the purpose and signature of the comparison function required by `qsort()`.  
How does it work, and why must it take `const void *` arguments?

---

### **Feature 6 – v1.5.0: Colorized Output**

**Q1: How do ANSI escape codes work to produce color in a standard Linux terminal? Show the specific code sequence for printing text in green.**
ANSI escape codes are special sequences of characters that tell the terminal to change text style, color, or formatting. They usually start with the escape character `\033[` followed by parameters and end with the letter `m`.
For example, to print text in **green**, you can use:

```c
printf("\033[0;32mThis text is green!\033[0m\n");
```

Here:

* `\033[` → begins the escape sequence
* `0;32m` → sets normal text style (0) and green foreground color (32)
* `\033[0m` → resets the color to default

---

**Q2: To color an executable file, you need to check its permission bits. Explain which bits in the `st_mode` field you need to check to determine if a file is executable by the owner, group, or others.**
In the `st_mode` field of the `stat` structure, the **execute permission bits** are:

* `S_IXUSR` → executable by **owner**
* `S_IXGRP` → executable by **group**
* `S_IXOTH` → executable by **others**

If any of these bits are set (checked using bitwise AND `&`), the file can be considered executable.
Example:

```c
if (fileStat.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
    // File is executable
}
```

---
