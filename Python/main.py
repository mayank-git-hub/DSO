import click
from src.pipeline_manager import PipelineManager
from src.logger import Logger

@click.group()
def main():
	pass
	
@main.command()
def RPI():
    pipeline_manager.server()

@main.command()
def PC():
    pipeline_manager.client()

if __name__ == "__main__":

	pipeline_manager = PipelineManager()
	log = Logger()
	log.first()
	
	main()