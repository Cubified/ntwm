/*
 * tiling.h: tiling utilities
 */

#ifndef __TILING_H
#define __TILING_H

static void tile(arr *array);

void tile(arr *array){
	int clients_count, i,
		cols, rows,
		col_height, col_width,
		col_number, row_number,
		x, y,
		w, h,
		gaps;
	monitor *current_monitor;

	clients_count = array->count;
	if(clients_count == 0){
		return;
	}

	for(cols = 0; cols <= clients_count / 2; cols++){
		if(cols*cols >= clients_count){
			break;
		}
	}

	gaps = (gaps_enabled ? GAPS : 0);

	rows = clients_count / cols;
	current_monitor = find_monitor();

	if(current_monitor == NULL){
		error("Could not locate monitor.");
	}

	col_height = current_monitor->height - gaps;
	col_width = (current_monitor->width - gaps) / (cols ? cols : 1)+2;
	col_number = 0;
	row_number = 0;

	for(i=0;i<clients_count;i++){
		if(i / rows + 1 > cols - clients_count % cols){
			rows = clients_count / cols + 1;
		}

		x = (col_number * col_width) + gaps + current_monitor->x;
		y = row_number * col_height / rows + gaps + current_monitor->y;
		w = col_width - gaps;
		h = (col_height / rows) - gaps;

		move_resize(
			array->data[i],
			x, y,
			w, h
		);

		if(++row_number >= rows){
			row_number = 0;
			col_number++;
		}
	}
}
#endif
