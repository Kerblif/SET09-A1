import csv
import matplotlib.pyplot as plt
import numpy as np
import os

svg_dir = 'svg_plots'
if not os.path.exists(svg_dir):
    os.makedirs(svg_dir)

data = {
    'algorithms': [],
    'data_types': [],
    'data_sizes': [],
    'time_ms': {},
    'comparisons': {},
    'verified_status': {}
}

csv_file_path = 'sort_performance.csv'
if not os.path.exists(csv_file_path):
    print(f"Ошибка: Файл {csv_file_path} не найден. Запустите бенчмарк для его генерации.")
    exit()

unverified_runs = []

with open(csv_file_path, 'r') as f:
    reader = csv.DictReader(f)
    if not reader.fieldnames:
        print(f"Ошибка: CSV файл {csv_file_path} пуст или некорректен (нет заголовков).")
        exit()

    expected_headers = ['Algorithm', 'DataType', 'DataSize', 'Time_ms', 'Comparisons', 'Verified']
    if not all(header in reader.fieldnames for header in expected_headers):
        print(f"Ошибка: CSV файл {csv_file_path} имеет неверные заголовки.")
        print(f"Ожидались: {expected_headers}")
        print(f"Получены: {reader.fieldnames}")
        exit()

    for row in reader:
        try:
            algorithm = row['Algorithm']
            data_type = row['DataType']
            data_size = int(row['DataSize'])
            time_ms_val = float(row['Time_ms'])
            comparisons_val = int(row['Comparisons'])
            verified = row['Verified'].lower() == 'true'
        except ValueError as e:
            print(f"Ошибка при чтении строки из CSV: {row} - {e}")
            continue
        except KeyError as e:
            print(f"Ошибка: отсутствует ожидаемый столбец {e} в строке: {row}")
            continue

        if algorithm not in data['algorithms']:
            data['algorithms'].append(algorithm)
            data['time_ms'][algorithm] = {}
            data['comparisons'][algorithm] = {}
            data['verified_status'][algorithm] = {}

        if data_type not in data['data_types']:
            data['data_types'].append(data_type)

        if data_size not in data['data_sizes']:
            data['data_sizes'].append(data_size)

        if not verified:
            unverified_runs.append(f"Algorithm: {algorithm}, DataType: {data_type}, Size: {data_size}")

data['data_sizes'].sort()

for algo in data['algorithms']:
    for data_type_key in data['data_types']:
        data['time_ms'][algo][data_type_key] = [0.0] * len(data['data_sizes'])
        data['comparisons'][algo][data_type_key] = [0] * len(data['data_sizes'])
        data['verified_status'][algo][data_type_key] = [True] * len(data['data_sizes'])

with open(csv_file_path, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        try:
            algorithm = row['Algorithm']
            data_type = row['DataType']
            data_size = int(row['DataSize'])
            time_ms_val = float(row['Time_ms'])
            comparisons_val = int(row['Comparisons'])
            verified = row['Verified'].lower() == 'true'
        except (ValueError, KeyError):
            continue

        if data_size not in data['data_sizes']:
            continue

        size_index = data['data_sizes'].index(data_size)

        if algorithm in data['time_ms'] and data_type in data['time_ms'][algorithm]:
            data['time_ms'][algorithm][data_type][size_index] = time_ms_val
        if algorithm in data['comparisons'] and data_type in data['comparisons'][algorithm]:
            data['comparisons'][algorithm][data_type][size_index] = comparisons_val
        if algorithm in data['verified_status'] and data_type in data['verified_status'][algorithm]:
            data['verified_status'][algorithm][data_type][size_index] = verified

if unverified_runs:
    print("\nПРЕДУПРЕЖДЕНИЕ: Обнаружены запуски, не прошедшие верификацию:")
    for run_info in unverified_runs:
        print(f"  - {run_info}")
    print("-" * 30)

algorithms = data['algorithms']
data_types = data['data_types']
data_sizes_list = data['data_sizes']
time_data = data['time_ms']
comparisons_data = data['comparisons']
verified_data = data['verified_status']


def plot_with_verification(ax, x_values, y_values, label, algo, data_type_key):
    line, = ax.plot(x_values, y_values, marker='o', label=label)
    color = line.get_color()

    if algo in verified_data and data_type_key in verified_data[algo]:
        statuses = verified_data[algo][data_type_key]
        for i, verified_status in enumerate(statuses):
            if not verified_status and i < len(x_values) and i < len(y_values):
                ax.scatter(x_values[i], y_values[i], color='red', marker='x', s=100, zorder=5,
                           label='Verification Failed' if 'Verification Failed' not in [l.get_label() for l in
                                                                                        ax.lines + ax.collections] else "")


plt.figure(figsize=(16, 20))
num_data_types = len(data_types)

for i, data_type_key in enumerate(data_types):
    ax_time = plt.subplot(num_data_types, 2, i * 2 + 1)
    for algo in algorithms:
        if algo in time_data and data_type_key in time_data[algo]:
            plot_with_verification(ax_time, data_sizes_list, time_data[algo][data_type_key], algo, algo, data_type_key)
        else:
            print(f"Предупреждение: Нет данных времени для {algo} на {data_type_key}")

    ax_time.set_title(f'Время выполнения для {data_type_key}')
    ax_time.set_xlabel('Размер данных (N)')
    ax_time.set_ylabel('Время (мс)')
    ax_time.grid(True)
    ax_time.legend()

    ax_comp = plt.subplot(num_data_types, 2, i * 2 + 2)
    for algo in algorithms:
        if algo in comparisons_data and data_type_key in comparisons_data[algo]:
            plot_with_verification(ax_comp, data_sizes_list, comparisons_data[algo][data_type_key], algo, algo,
                                   data_type_key)
        else:
            print(f"Предупреждение: Нет данных сравнений для {algo} на {data_type_key}")

    ax_comp.set_title(f'Количество сравнений для {data_type_key}')
    ax_comp.set_xlabel('Размер данных (N)')
    ax_comp.set_ylabel('Количество сравнений')
    ax_comp.grid(True)
    ax_comp.legend()

plt.tight_layout()
plt.savefig('sorting_performance_combined.png', dpi=300, bbox_inches='tight')
plt.savefig(f'{svg_dir}/sorting_performance_combined.svg', format='svg', bbox_inches='tight')
plt.close()

plt.figure(figsize=(16, 20))
for i, data_type_key in enumerate(data_types):
    # График времени (лог)
    ax_time_log = plt.subplot(num_data_types, 2, i * 2 + 1)
    for algo in algorithms:
        if algo in time_data and data_type_key in time_data[algo]:
            current_times = np.array(time_data[algo][data_type_key])
            current_sizes = np.array(data_sizes_list)
            valid_indices = current_times > 0

            if np.any(valid_indices):
                plot_with_verification(ax_time_log, current_sizes[valid_indices], current_times[valid_indices], algo,
                                       algo, data_type_key)

    ax_time_log.set_title(f'Время выполнения для {data_type_key} (лог. шкала)')
    ax_time_log.set_xlabel('Размер данных (N)')
    ax_time_log.set_ylabel('Время (мс)')
    ax_time_log.set_yscale('log')
    ax_time_log.grid(True, which="both", ls="-")
    ax_time_log.legend()

    ax_comp_log = plt.subplot(num_data_types, 2, i * 2 + 2)
    for algo in algorithms:
        if algo in comparisons_data and data_type_key in comparisons_data[algo]:
            current_comps = np.array(comparisons_data[algo][data_type_key])
            current_sizes = np.array(data_sizes_list)
            valid_indices = current_comps > 0
            if np.any(valid_indices):
                plot_with_verification(ax_comp_log, current_sizes[valid_indices], current_comps[valid_indices], algo,
                                       algo, data_type_key)

    ax_comp_log.set_title(f'Количество сравнений для {data_type_key} (лог. шкала)')
    ax_comp_log.set_xlabel('Размер данных (N)')
    ax_comp_log.set_ylabel('Количество сравнений')
    ax_comp_log.set_yscale('log')
    ax_comp_log.grid(True, which="both", ls="-")
    ax_comp_log.legend()

plt.tight_layout()
plt.savefig('sorting_performance_log_combined.png', dpi=300, bbox_inches='tight')
plt.savefig(f'{svg_dir}/sorting_performance_log_combined.svg', format='svg', bbox_inches='tight')
plt.close()

for data_type_key in data_types:
    plt.figure(figsize=(10, 6))
    ax_time_ind = plt.gca()
    for algo in algorithms:
        if algo in time_data and data_type_key in time_data[algo]:
            plot_with_verification(ax_time_ind, data_sizes_list, time_data[algo][data_type_key], algo, algo,
                                   data_type_key)
    ax_time_ind.set_title(f'Время выполнения для {data_type_key}')
    ax_time_ind.set_xlabel('Размер данных (N)')
    ax_time_ind.set_ylabel('Время (мс)')
    ax_time_ind.grid(True)
    ax_time_ind.legend()
    plt.tight_layout()
    plt.savefig(f'{svg_dir}/time_{data_type_key.replace(" ", "_")}.svg', format='svg', bbox_inches='tight')
    plt.close()

    plt.figure(figsize=(10, 6))
    ax_time_ind_log = plt.gca()
    for algo in algorithms:
        if algo in time_data and data_type_key in time_data[algo]:
            current_times = np.array(time_data[algo][data_type_key])
            current_sizes = np.array(data_sizes_list)
            valid_indices = current_times > 0
            if np.any(valid_indices):
                plot_with_verification(ax_time_ind_log, current_sizes[valid_indices], current_times[valid_indices],
                                       algo, algo, data_type_key)
    ax_time_ind_log.set_title(f'Время выполнения для {data_type_key} (лог. шкала)')
    ax_time_ind_log.set_xlabel('Размер данных (N)')
    ax_time_ind_log.set_ylabel('Время (мс)')
    ax_time_ind_log.set_yscale('log')
    ax_time_ind_log.grid(True, which="both", ls="-")
    ax_time_ind_log.legend()
    plt.tight_layout()
    plt.savefig(f'{svg_dir}/time_{data_type_key.replace(" ", "_")}_log.svg', format='svg', bbox_inches='tight')
    plt.close()

    plt.figure(figsize=(10, 6))
    ax_comp_ind = plt.gca()
    for algo in algorithms:
        if algo in comparisons_data and data_type_key in comparisons_data[algo]:
            plot_with_verification(ax_comp_ind, data_sizes_list, comparisons_data[algo][data_type_key], algo, algo,
                                   data_type_key)
    ax_comp_ind.set_title(f'Количество сравнений для {data_type_key}')
    ax_comp_ind.set_xlabel('Размер данных (N)')
    ax_comp_ind.set_ylabel('Количество сравнений')
    ax_comp_ind.grid(True)
    ax_comp_ind.legend()
    plt.tight_layout()
    plt.savefig(f'{svg_dir}/comparisons_{data_type_key.replace(" ", "_")}.svg', format='svg', bbox_inches='tight')
    plt.close()

    plt.figure(figsize=(10, 6))
    ax_comp_ind_log = plt.gca()
    for algo in algorithms:
        if algo in comparisons_data and data_type_key in comparisons_data[algo]:
            current_comps = np.array(comparisons_data[algo][data_type_key])
            current_sizes = np.array(data_sizes_list)
            valid_indices = current_comps > 0
            if np.any(valid_indices):
                plot_with_verification(ax_comp_ind_log, current_sizes[valid_indices], current_comps[valid_indices],
                                       algo, algo, data_type_key)
    ax_comp_ind_log.set_title(f'Количество сравнений для {data_type_key} (лог. шкала)')
    ax_comp_ind_log.set_xlabel('Размер данных (N)')
    ax_comp_ind_log.set_ylabel('Количество сравнений')
    ax_comp_ind_log.set_yscale('log')
    ax_comp_ind_log.grid(True, which="both", ls="-")
    ax_comp_ind_log.legend()
    plt.tight_layout()
    plt.savefig(f'{svg_dir}/comparisons_{data_type_key.replace(" ", "_")}_log.svg', format='svg', bbox_inches='tight')
    plt.close()

print("\nГрафики сохранены в директорию 'svg_plots' и в корне проекта (PNG).")

print("\nAlgorithmic Complexity Analysis (Time):")
print("======================================")
for algo in algorithms:
    print(f"\n{algo}:")
    for data_type_key in data_types:
        if algo not in time_data or data_type_key not in time_data[algo]:
            print(f"  {data_type_key}: Данные отсутствуют.")
            continue

        times = np.array(time_data[algo][data_type_key])
        current_data_sizes = np.array(data_sizes_list)

        valid_indices = times > 1e-9

        if np.sum(valid_indices) < 2:
            print(f"  {data_type_key}: Недостаточно данных для анализа сложности (точек < 2 или времена слишком малы).")
            continue

        times_filtered = times[valid_indices]
        sizes_filtered = current_data_sizes[valid_indices]

        if len(sizes_filtered) >= 2 and len(times_filtered) >= 2:
            log_sizes = np.log(sizes_filtered)
            log_times = np.log(times_filtered)

            if np.any(np.isinf(log_sizes)) or np.any(np.isnan(log_sizes)) or \
                    np.any(np.isinf(log_times)) or np.any(np.isnan(log_times)):
                print(f"  {data_type_key}: Некорректные значения после логарифмирования, пропуск анализа.")
                continue

            try:
                coeffs = np.polyfit(log_sizes, log_times, 1)
                power = coeffs[0]
            except (np.linalg.LinAlgError, ValueError) as e:
                print(f"  {data_type_key}: Ошибка при расчете polyfit: {e}. Пропуск анализа.")
                continue

            complexity_str = "Unknown"
            if 0.8 <= power <= 1.2:
                ratios_nlogn = times_filtered / (sizes_filtered * np.log(sizes_filtered))
                if len(ratios_nlogn) > 1 and np.std(ratios_nlogn) / np.mean(ratios_nlogn) < 0.3:
                    complexity_str = "O(N log N)"
                else:
                    complexity_str = "O(N)"
            elif 1.2 < power <= 1.7:
                complexity_str = "O(N log N)"
            elif 1.8 <= power <= 2.2:
                complexity_str = "O(N^2)"
            elif 0.0 <= power < 0.8:
                complexity_str = "Sub-linear or O(log N)"
            else:
                complexity_str = f"O(N^{power:.1f})"

            print(f"  {data_type_key}: Estimated power = {power:.2f} -> Approx. Complexity: {complexity_str}")

        else:
            print(f"  {data_type_key}: Недостаточно валидных точек для анализа сложности.")
