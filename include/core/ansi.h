#pragma once

namespace core {

#define ANSI_RESET() "\x1b[0m" // Reset all attributes

#define ANSI_BLACK_START() "\x1b[30m"
#define ANSI_BLACK(text) ANSI_BLACK_START() text ANSI_RESET()
#define ANSI_RED_START() "\x1b[31m"
#define ANSI_RED(text) ANSI_RED_START() text ANSI_RESET()
#define ANSI_GREEN_START() "\x1b[32m"
#define ANSI_GREEN(text) ANSI_GREEN_START() text ANSI_RESET()
#define ANSI_YELLOW_START() "\x1b[33m"
#define ANSI_YELLOW(text) ANSI_YELLOW_START() text ANSI_RESET()
#define ANSI_BLUE_START() "\x1b[34m"
#define ANSI_BLUE(text) ANSI_BLUE_START() text ANSI_RESET()
#define ANSI_MAGENTA_START() "\x1b[35m"
#define ANSI_MAGENTA(text) ANSI_MAGENTA_START() text ANSI_RESET()
#define ANSI_CYAN_START() "\x1b[36m"
#define ANSI_CYAN(text) ANSI_CYAN_START() text ANSI_RESET()
#define ANSI_WHITE_START() "\x1b[37m"
#define ANSI_WHITE(text) ANSI_WHITE_START() text ANSI_RESET()

#define ANSI_BRIGHT_BLACK_START() "\x1b[90m"
#define ANSI_BRIGHT_BLACK(text) ANSI_BRIGHT_BLACK_START() text ANSI_RESET()
#define ANSI_BRIGHT_RED_START() "\x1b[91m"
#define ANSI_BRIGHT_RED(text) ANSI_BRIGHT_RED_START() text ANSI_RESET()
#define ANSI_BRIGHT_GREEN_START() "\x1b[92m"
#define ANSI_BRIGHT_GREEN(text) ANSI_BRIGHT_GREEN_START() text ANSI_RESET()
#define ANSI_BRIGHT_YELLOW_START() "\x1b[93m"
#define ANSI_BRIGHT_YELLOW(text) ANSI_BRIGHT_YELLOW_START() text ANSI_RESET()
#define ANSI_BRIGHT_BLUE_START() "\x1b[94m"
#define ANSI_BRIGHT_BLUE(text) ANSI_BRIGHT_BLUE_START() text ANSI_RESET()
#define ANSI_BRIGHT_MAGENTA_START() "\x1b[95m"
#define ANSI_BRIGHT_MAGENTA(text) ANSI_BRIGHT_MAGENTA_START() text ANSI_RESET()
#define ANSI_BRIGHT_CYAN_START() "\x1b[96m"
#define ANSI_BRIGHT_CYAN(text) ANSI_BRIGHT_CYAN_START() text ANSI_RESET()
#define ANSI_BRIGHT_WHITE_START() "\x1b[97m"
#define ANSI_BRIGHT_WHITE(text) ANSI_BRIGHT_WHITE_START() text ANSI_RESET()

#define ANSI_BACKGROUND_BLACK_START() "\x1b[40m"
#define ANSI_BACKGROUND_BLACK(text) ANSI_BACKGROUND_BLACK_START() text ANSI_RESET()
#define ANSI_BACKGROUND_RED_START() "\x1b[41m"
#define ANSI_BACKGROUND_RED(text) ANSI_BACKGROUND_RED_START() text ANSI_RESET()
#define ANSI_BACKGROUND_GREEN_START() "\x1b[42m"
#define ANSI_BACKGROUND_GREEN(text) ANSI_BACKGROUND_GREEN_START() text ANSI_RESET()
#define ANSI_BACKGROUND_YELLOW_START() "\x1b[43m"
#define ANSI_BACKGROUND_YELLOW(text) ANSI_BACKGROUND_YELLOW_START() text ANSI_RESET()
#define ANSI_BACKGROUND_BLUE_START() "\x1b[44m"
#define ANSI_BACKGROUND_BLUE(text) ANSI_BACKGROUND_BLUE_START() text ANSI_RESET()
#define ANSI_BACKGROUND_MAGENTA_START() "\x1b[45m"
#define ANSI_BACKGROUND_MAGENTA(text) ANSI_BACKGROUND_MAGENTA_START() text ANSI_RESET()
#define ANSI_BACKGROUND_CYAN_START() "\x1b[46m"
#define ANSI_BACKGROUND_CYAN(text) ANSI_BACKGROUND_CYAN_START() text ANSI_RESET()
#define ANSI_BACKGROUND_WHITE_START() "\x1b[47m"
#define ANSI_BACKGROUND_WHITE(text) ANSI_BACKGROUND_WHITE_START() text ANSI_RESET()

#define ANSI_BRIGHT_BACKGROUND_BLACK_START() "\x1b[100m"
#define ANSI_BRIGHT_BACKGROUND_BLACK(text) ANSI_BRIGHT_BACKGROUND_BLACK_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_RED_START() "\x1b[101m"
#define ANSI_BRIGHT_BACKGROUND_RED(text) ANSI_BRIGHT_BACKGROUND_RED_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_GREEN_START() "\x1b[102m"
#define ANSI_BRIGHT_BACKGROUND_GREEN(text) ANSI_BRIGHT_BACKGROUND_GREEN_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_YELLOW_START() "\x1b[103m"
#define ANSI_BRIGHT_BACKGROUND_YELLOW(text) ANSI_BRIGHT_BACKGROUND_YELLOW_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_BLUE_START() "\x1b[104m"
#define ANSI_BRIGHT_BACKGROUND_BLUE(text) ANSI_BRIGHT_BACKGROUND_BLUE_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_MAGENTA_START() "\x1b[105m"
#define ANSI_BRIGHT_BACKGROUND_MAGENTA(text) ANSI_BRIGHT_BACKGROUND_MAGENTA_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_CYAN_START() "\x1b[106m"
#define ANSI_BRIGHT_BACKGROUND_CYAN(text) ANSI_BRIGHT_BACKGROUND_CYAN_START() text ANSI_RESET()
#define ANSI_BRIGHT_BACKGROUND_WHITE_START() "\x1b[107m"
#define ANSI_BRIGHT_BACKGROUND_WHITE(text) ANSI_BRIGHT_BACKGROUND_WHITE_START() text ANSI_RESET()

#define ANSI_BOLD_START() "\x1b[1m"
#define ANSI_BOLD(text) ANSI_BOLD_START() text ANSI_RESET()
#define ANSI_DIM_START() "\x1b[2m"
#define ANSI_DIM(text) ANSI_DIM_START() text ANSI_RESET()
#define ANSI_ITALIC_START() "\x1b[3m"
#define ANSI_ITALIC(text) ANSI_ITALIC_START() text ANSI_RESET()
#define ANSI_UNDERLINE_START() "\x1b[4m"
#define ANSI_UNDERLINE(text) ANSI_UNDERLINE_START() text ANSI_RESET()
#define ANSI_BLINK_START() "\x1b[5m"
#define ANSI_BLINK(text) ANSI_BLINK_START() text ANSI_RESET()
#define ANSI_RAPID_BLINK_START() "\x1b[6m"
#define ANSI_RAPID_BLINK(text) ANSI_RAPID_BLINK_START() text ANSI_RESET()
#define ANSI_REVERSE_START() "\x1b[7m"
#define ANSI_REVERSE(text) ANSI_REVERSE_START() text ANSI_RESET()
#define ANSI_HIDDEN_START() "\x1b[8m"
#define ANSI_HIDDEN(text) ANSI_HIDDEN_START() text ANSI_RESET()
#define ANSI_STRIKETHROUGH_START() "\x1b[9m"
#define ANSI_STRIKETHROUGH(text) ANSI_STRIKETHROUGH_START() text ANSI_RESET()

} // namespace  core
