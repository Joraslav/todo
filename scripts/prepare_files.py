import json
import random
from datetime import UTC, datetime, timedelta
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
TEST_DIR = PROJECT_ROOT / "tests"
STORAGE_DIR = TEST_DIR / "storage"
CONFIG_FILE = TEST_DIR / "config.json"
STORAGE_NAME = "tasks.json"

TASK_NAMES: tuple[str, ...] = (
    "Buy products",
    "Walk with dog",
    "Clone code",
)


def _random_iso_datetime() -> str:
    base = datetime(2026, 1, 1, tzinfo=UTC)
    offset = timedelta(
        days=random.randint(0, 365),
        hours=random.randint(0, 23),
        minutes=random.randint(0, 59),
        seconds=random.randint(0, 59),
    )
    return (base + offset).strftime("%Y-%m-%dT%H:%M:%S")


def _make_task() -> dict:
    return {
        "name": random.choice(TASK_NAMES),
        "is_done": random.choice((True, False)),
        "date": _random_iso_datetime() if random.random() < 0.6 else None,
    }


def main() -> None:
    TEST_DIR.mkdir(parents=True, exist_ok=True)
    STORAGE_DIR.mkdir(parents=True, exist_ok=True)

    config = {
        "storage_dir": "./tests/storage",
        "storage_name": STORAGE_NAME,
    }
    CONFIG_FILE.write_text(
        json.dumps(config, indent=4, ensure_ascii=False), encoding="utf-8"
    )
    print(f"[Ok] {CONFIG_FILE.relative_to(PROJECT_ROOT)} is written")

    tasks = [_make_task() for _ in range(5)]
    storage_file = STORAGE_DIR / STORAGE_NAME
    storage_file.write_text(json.dumps(tasks, indent=4, ensure_ascii=False), encoding="utf-8")
    print(f"[Ok] {storage_file.relative_to(PROJECT_ROOT)} written 5 tasks")


if __name__ == "__main__":
    main()
